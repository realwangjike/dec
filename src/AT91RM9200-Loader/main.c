#include "main.h"

#define LOADER_VERSION	"loader 1.0"

extern char _stext,_etext,_sdata,_edata;

/* Global variables */
volatile char		XmodemComplete =0;
unsigned int 		StTick;

AT91S_RomBoot const *pAT91;
AT91S_SBuffer   	sXmBuffer;
AT91S_SvcXmodem 	svcXmodem;
AT91S_Pipe      	xmodemPipe;
AT91S_CtlTempo  	ctlTempo;

/*--------------------------------------------------------------------------------------*/
/* Function Name       : AT91F_SetPLL()							*/
/* Object              : Set the PLLA to 180MHz and Master Clock to 60Mhz		*/
/*--------------------------------------------------------------------------------------*/
void AT91F_SetPLL(void)
{
	volatile int tmp = 0;
	
	/* APMC Initialization for Crystal */
	AT91PS_PMC pApmc = (AT91PS_PMC)AT91C_BASE_PMC;
	AT91PS_CKGR pCkgr =  (AT91PS_CKGR)AT91C_BASE_CKGR;
	
	pApmc->PMC_IDR = 0xFFFFFFFF;
	/* -Setup the PLL A */
	pCkgr->CKGR_PLLAR = AT91C_PLLA_VALUE; 
	
	while(!(pApmc->PMC_SR & AT91C_PMC_LOCKA) && (tmp++ < DELAY_MAIN_FREQ));

	/* Write in the MCKR dirty value concerning the clock selection CSS then overwrite it in a second sequence */
	pApmc->PMC_MCKR = 0x203;
	/* Wait until the master clock is established */
	tmp = 0;
	while(!(pApmc->PMC_SR & AT91C_PMC_MCKRDY) && (tmp++ < DELAY_MAIN_FREQ));

	/* - Commuting Master Clock from PLLB to PLLA/3 */
	pApmc->PMC_MCKR = 0x202;
	/* Wait until the master clock is established */
	tmp = 0;
	while(!(pApmc->PMC_SR & AT91C_PMC_MCKRDY) && (tmp++ < DELAY_MAIN_FREQ));

	/* Setup MEMC to support all connected memories (CS0 = FLASH; CS1=SDRAM) */
	AT91C_BASE_EBI->EBI_CSA = AT91C_EBI_CS1A;

	/* com set CS0 cs for flash */
	AT91C_BASE_SMC2->SMC2_CSR[0] = 0x00003284;
}


/*--------------------------------------------------------------------------------------*/
/* Function Name       : AT91_XmodemComplete()						*/
/* Object              : Perform the remap and jump to appli in RAM			*/
/*--------------------------------------------------------------------------------------*/
void AT91F_XmodemComplete(AT91S_PipeStatus status, void *pVoid)
{
	/* stop the Xmodem tempo */
	svcXmodem.tempo.Stop(&(svcXmodem.tempo));
	XmodemComplete = 1;
}


/*--------------------------------------------------------------------------------------*/
/* Function Name       : AT91F_XmodemProtocol(AT91S_PipeStatus status, void *pVoid)	*/
/* Object              : Xmodem dispatcher						*/
/*--------------------------------------------------------------------------------------*/
void AT91F_XmodemProtocol(AT91S_PipeStatus status, void *pVoid)
{
	AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer) xmodemPipe.pBuffer->pChild;
	AT91PS_USART   pUsart     = svcXmodem.pUsart;
			
	if (pSBuffer->szRdBuffer == 0) {
		svcXmodem.tempo.Start(&(svcXmodem.tempo), 10, 0, AT91F_XmodemComplete, pUsart);								
	}
}


/*--------------------------------------------------------------------------------------*/
/* Function Name       : GetTickCount()							*/
/* Object              : Return the number of systimer tick 				*/
/*--------------------------------------------------------------------------------------*/
unsigned int AT91F_GetTickCount(void)
{
	return StTick;
}


/*-------------------------- Interrupt handlers ----------------------------------------*/
/*--------------------------------------------------------------------------------------*/
/* Function Name       : AT91F_ST_HANDLER()						*/
/* Object              : C Interrupt handler for the system timer			*/
/*--------------------------------------------------------------------------------------*/
void AT91F_ST_HANDLER(void)
{
	volatile unsigned int csr = *AT91C_DBGU_CSR;
	unsigned int error;
	
	/* ========== Systimer interrupt ============== */
	if (AT91C_BASE_ST->ST_SR & 0x01) {
		StTick++;
		ctlTempo.CtlTempoTick(&ctlTempo);
		return;
	}

	error = AT91F_US_Error((AT91PS_USART)AT91C_BASE_DBGU);
	if (csr & error) {
		/* Stop previous Xmodem transmition */
		*(AT91C_DBGU_CR) = AT91C_US_RSTSTA;
		AT91F_US_DisableIt((AT91PS_USART)AT91C_BASE_DBGU, AT91C_US_ENDRX);
		AT91F_US_EnableIt((AT91PS_USART)AT91C_BASE_DBGU, AT91C_US_RXRDY);

	}
	
	else if (csr & (AT91C_US_TXRDY | AT91C_US_ENDTX | AT91C_US_TXEMPTY | 
	                AT91C_US_RXRDY | AT91C_US_ENDRX | AT91C_US_TIMEOUT | 
	                AT91C_US_RXBUFF)) {
		if ( !(svcXmodem.eot) )
			svcXmodem.Handler(&svcXmodem, csr);
	}
}


/*----------------------------------------------------------------------------  */
/* Function Name       : main							*/
/* Object              : Main function						*/
/*----------------------------------------------------------------------------	*/
int boot(void)
{
	AT91PS_Buffer  pXmBuffer;
	AT91PS_SvcComm pSvcXmodem;
	unsigned int   SizeDownloaded;	

	/* Init of ROM services structure */
	pAT91 = AT91C_ROM_BOOT_ADDRESS;

	/* Tempo Initialization */
	pAT91->OpenCtlTempo(&ctlTempo, (void *) &(pAT91->SYSTIMER_DESC));
	ctlTempo.CtlTempoStart((void *) &(pAT91->SYSTIMER_DESC));

	/* Xmodem Initialization */
	pXmBuffer  = pAT91->OpenSBuffer(&sXmBuffer);
	pSvcXmodem = pAT91->OpenSvcXmodem(&svcXmodem, (AT91PS_USART)AT91C_BASE_DBGU, &ctlTempo);
	pAT91->OpenPipe(&xmodemPipe, pSvcXmodem, pXmBuffer);

	/* System Timer initialization */
	AT91F_AIC_ConfigureIt (	AT91C_BASE_AIC,                        /* AIC base address */
				AT91C_ID_SYS,                          /* System peripheral ID */
				AT91C_AIC_PRIOR_HIGHEST,               /* Max priority */
				AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE,   /* Level sensitive */
				AT91F_ST_ASM_HANDLER );						

	/* Enable ST interrupt */
	AT91F_AIC_EnableIt(AT91C_BASE_AIC, AT91C_ID_SYS);

	AT91F_DBGU_Printk(CLRSCREEN);

	AT91F_DBGU_Printk("n\r"LOADER_VERSION" ("__DATE__ " - " __TIME__ ")\n\n\r");
	
	AT91F_DBGU_Printk("XMODEM: Download U-BOOT\n\r");

	xmodemPipe.Read(&xmodemPipe, (char *)AT91C_UBOOT_BASE_ADDRESS, AT91C_UBOOT_MAXSIZE, AT91F_XmodemProtocol, 0);	
	while(XmodemComplete !=1);
		
	AT91F_DBGU_Printk("\n\rU-Boot downloaded successfully\n\r");

	SizeDownloaded = AT91F_GetTickCount() + AT91C_PAUSE;
	while(AT91F_GetTickCount() < SizeDownloaded);

	AT91F_SetPLL();

	AT91F_AIC_DisableIt(AT91C_BASE_AIC, 0xFFFFFFFF);
	AT91F_AIC_AcknowledgeIt(AT91C_BASE_AIC);

	Jump((unsigned int)AT91C_UBOOT_BASE_ADDRESS);
 }

