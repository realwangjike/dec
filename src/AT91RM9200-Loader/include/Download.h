//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : Download.h
//* Object              : 
//*
//* 1.0 23/03/01 	HI  : Creation
//*----------------------------------------------------------------------------

#define AT91C_DATAFLASH_OK			1
#define AT91C_EEPROM_OK				2
#define AT91C_EXTERNMEM_OK			3

extern int DownloadEntry(void);
extern int EepromBoot(void);
extern int DataflashBoot( void);
extern int ExternMemBoot(void);
