//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : obj_romboot_h
//* Object              :
//*
//* 1.0 Oct 11th  HI ODi : Creation
//* 1.1 Oct 2nd   FB : Dataflash service add on
//*----------------------------------------------------------------------------
#ifndef obj_romboot_h
#define obj_romboot_h

#ifdef THUNDER
#include "parts/thunder/lib_thunder.h"
#endif // THUNDER

#ifdef BOOSTER_LIGHT
#include "parts/Booster_light/lib_booster_light.h"
#endif // BOOSTER_LIGHT

#include "services/obj_pipe.h"
#include "services/obj_sbuffer.h"
#include "services/obj_svcudp.h"
#include "services/obj_svcxmodem.h"
#include "services/obj_romboot.h"
#include "services/obj_svctempo.h"
#include "services/obj_usbdesc.h"
#include "services/obj_dfudesc.h"
#include "services/obj_svcdataflash.h"

#include "appli/includes/crc.h"
#include "appli/includes/sinus.h"

typedef struct _AT91S_RomBoot {
	const unsigned int     version;
	// Peripheral descriptors
	const AT91S_MEMCDesc   MEMC_DESC;
	const AT91S_STDesc     SYSTIMER_DESC;
	const AT91S_Pio2Desc   PIOA_DESC;
	const AT91S_Pio2Desc   PIOB_DESC;
	const AT91S_USART2Desc DBGU_DESC;
	const AT91S_USART2Desc USART0_DESC;
	const AT91S_USART2Desc USART1_DESC;
	const AT91S_USART2Desc USART2_DESC;
	const AT91S_USART2Desc USART3_DESC;
	const AT91S_TWIDesc    TWI_DESC;
	const AT91S_SPIDesc    SPI_DESC;

	// Objects entry
	const AT91PF_OpenPipe      		OpenPipe;
	const AT91PF_OpenSBuffer   		OpenSBuffer;
	const AT91PF_OpenSvcUdp    		OpenSvcUdp;
	const AT91PF_OpenSvcXmodem 		OpenSvcXmodem;
	const AT91PF_OpenCtlTempo  		OpenCtlTempo;
	const AT91PF_OpenDfuDesc   		OpenDfuDesc;
	const AT91PF_OpenUsbDesc   		OpenUsbDesc;
	const AT91PF_OpenSvcDataFlash 	OpenSvcDataFlash;
	const AT91PF_SVC_CRC16			CRC16;
	const AT91PF_SVC_CRCCCITT		CRCCCITT;
	const AT91PF_SVC_CRCHDLC		CRCHDLC;
	const AT91PF_SVC_CRC32			CRC32;
	
	// Array
	const AT91PS_SVC_CRC_BIT_REV	Bit_Reverse_Array;
	const AT91PS_SINE_TAB			SineTab;
	const AT91PF_Sinus              Sine;
 
} AT91S_RomBoot, *AT91PS_RomBoot;

#define AT91C_ROM_BOOT_ADDRESS ((const AT91S_RomBoot *) ( *((unsigned int *) (AT91C_ROM_BASE + 0x20))) )

#endif // obj_romboot_h