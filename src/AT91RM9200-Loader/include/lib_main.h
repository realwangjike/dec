//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_main.h
//* Object              : Test for new boot validation
//*
//* 1.0 09/10/02 FB     : Creation
//*----------------------------------------------------------------------------

#ifndef lib_main_h
#define lib_main_h

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgPullup
//* \brief Enable pullup on PIO
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_CfgPullup(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int pullupEnable)   // \arg enable pullup on PIO
{
		// Connect or not Pullup
	pPio->PIO_PPUDR = ~pullupEnable;
	pPio->PIO_PPUER = pullupEnable;
}


//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgPeriph
//* \brief Enable pins to be drived by peripheral
//*----------------------------------------------------------------------------
__inline void AT91F_PIO_CfgPeriph(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int periphAEnable,  // \arg PERIPH A to enable
	unsigned int periphBEnable)  // \arg PERIPH B to enable

{
	pPio->PIO_ASR = periphAEnable;
	pPio->PIO_BSR = periphBEnable;
	pPio->PIO_PDR = (periphAEnable | periphBEnable); // Set in Periph mode
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_CfgPIO
//* \brief Configure PIO controllers to drive SPI signals
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
						 PIOA_BASE, // PIO controller base address
						 ((unsigned int) (1<<PIONPCS0)) |
						 ((unsigned int) (1<<PIOSPCK))  |
						 ((unsigned int) (1<<PIOMOSI))  |
						 ((unsigned int) (1<<PIOMISO)),
						 0
						); // Peripheral B	
		
	AT91F_PIO_CfgPullup(
						 PIOA_BASE,
						 ((unsigned int) (1<<PIONPCS0)) |
						 ((unsigned int) (1<<PIOMISO))
						);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_EnablePeriphClock
//* \brief Enable peripheral clock
//*----------------------------------------------------------------------------
__inline void AT91F_APMC_EnablePeriphClock (
	AT91PS_APMC pAPMC, // \arg pointer to PMC controller
	unsigned int periphIds)  // \arg IDs of peripherals to enable
{
	pAPMC->APMC_PCER = periphIds;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_CfgPMC
//* \brief Enable Peripheral clock in PMC for  SPI
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_CfgPMC (void)
{
	AT91F_APMC_EnablePeriphClock(
		APMC_BASE, // PIO controller base address
		((unsigned int) 1 << SPI_ID));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_CfgSPI
//* \brief Config SPI IP
//*----------------------------------------------------------------------------
__inline void AT91F_SPI_CfgSPI (void)
{
	//* Reset the SPI
    SPI_BASE->SP_CR = SP_SWRST ;
    //* Enable the SPI
    SPI_BASE->SP_CR = SP_SPIEN ;
    
    SPI_BASE->SP_MR = SP_MSTR | SP_MODEFAULT_DIS | SP_PCS0;
    
    SPI_BASE->SP_CSR[0] = (SP_CPOL | (16 << 16) | (SPI_CLK << 8));
    
    //* Enable the PDC transfert
	SPI_BASE->SP_PTCR = PDC_TXTEN + PDC_RXTEN;
}
#endif