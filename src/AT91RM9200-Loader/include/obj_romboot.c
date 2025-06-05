//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : obj_romboot.c
//* Object              :
//*
//* 1.0 Oct 11th  HI ODi : Creation
//*----------------------------------------------------------------------------
#include "at91lib_version.h"
#include "services/obj_romboot.h"

const AT91S_RomBoot AT91I_RomBoot = {
	AT91C_LIB_VERSION,
	// Memory controler
	{
		MEMC_BASE ,
		MC_ID         /* Peripheral Identifier */
	},

	// System timer
	{
		ST_BASE ,
		(void *)0 ,      /* Assembly interrupt handler */
		SYS_ID         	 /* Peripheral Identifier */
	},

	// Pio A
	{
		PIOA_BASE,
		PIOA_ID,
		NB_PIOA
	},

	// PIO B
	{
		PIOB_BASE,
		PIOB_ID,
		NB_PIOB
	},

	// DBGU
	{
	    DBGU_BASE,
	    PIOA_BASE,
	    PIODRXD,
	    PIODTXD,
	    PIOUNUSED,
	    PIOUNUSED,
	    PIOUNUSED,
	    PIOUNUSED,
	    PIOUNUSED,
	    PIOUNUSED,
	    PIOUNUSED,
	    DBGU_ID
	},

	// USART 0
	{
	    USART0_BASE,
	    PIOA_BASE,
	    PIORXD0,
	    PIOTXD0,
	    PIOSCK0,
	    PIORTS0,
	    PIOCTS0,
	    PIOUNUSED,
	    PIOUNUSED,
	    PIOUNUSED,
	    PIOUNUSED,
	    US0_ID
	},

	// USART 1
	{
		USART1_BASE,
		PIOA_BASE,
		PIORXD1,
	 	PIOTXD1,
		PIOSCK1,
	 	PIORTS1,
	 	PIOCTS1,
		PIODTR1,
		PIORI1,
		PIODSR1,
		PIODCD1,
		US1_ID
	},

	// USART 2
	{
		USART2_BASE,
		PIOA_BASE,
		PIORXD2,
		PIOTXD2,
		PIOSCK2,
		PIORTS2,
		PIOCTS2,
		PIOUNUSED,
		PIOUNUSED,
		PIOUNUSED,
		PIOUNUSED,
		US2_ID
	},

	// USART 3
	{
		USART3_BASE,
		PIOB_BASE,
		PIORXD3,
		PIOTXD3,
		PIOSCK3,
		PIORTS3,
		PIOCTS3,
		PIOUNUSED,
		PIOUNUSED,
		PIOUNUSED,
		PIOUNUSED,
		US3_ID
	},

	// TWI
	{
		TWI_BASE,
		PIOA_BASE,
		PIOSDA,		/* TWI Data I/O */
		PIOSCL,		/* TWI Clock */
		TWI_ID
	},

	// SPI
	{
		SPI_BASE,
		PIOA_BASE,
		SPI_ID,
		PIOSPCK,
		PIOMISO,
		PIOMOSI,
		{
			PIONPCS0,
			PIONPCS1,
			PIONPCS2,
			PIONPCS3
		}
	},

	AT91F_OpenPipe,
	AT91F_OpenSBuffer,
	AT91F_OpenSvcUdp,
	AT91F_OpenSvcXmodem,
	AT91F_OpenCtlTempo,
	AT91F_OpenDfuDesc,
	AT91F_OpenUsbDesc,
	AT91F_OpenSvcDataFlash,
	CalculateCrc16,
	CalculateCrc16ccitt,
	CalculateCrcHdlc,
	CalculateCrc32,
	bit_rev,
	AT91C_SINUS180_TAB,
	AT91F_Sinus,
};