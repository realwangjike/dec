//*---------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*---------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*---------------------------------------------------------------------------
//* File Name           : AT91C_SPI_DataFlash.h
//* Object              : Data Flash Atmel Description File
//* Translator          :
//*
//* 1.0 03/04/01 HI	: Creation
//* 1.2 20/10/02 FB	: Adapatation Service and Lib v3
//*---------------------------------------------------------------------------

#ifndef AT91C_DataFlash_h
#define AT91C_DataFlash_h

/////////////////////////////////////////////////////////////////////////////////////////////////////

/*DataFlash return value */
#define DATAFLASH_BUSY				0x00
#define DATAFLASH_OK				0x01
#define DATAFLASH_ERROR				0x02
#define DATAFLASH_MEMORY_OVERFLOW	0x03
#define DATAFLASH_BAD_COMMAND		0x04
#define DATAFLASH_BAD_ADDRESS		0x05

#define DATAFLASH_CLK		12000000
#define DATAFLASH_TIMEOUT	30
/* Driver State */
#define IDLE				0x0
#define	BUSY				0x1
#define	ERROR				0x2

/* DataFlash Driver State */
#define	GET_STATUS			0x0F

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Command Definition
/////////////////////////////////////////////////////////////////////////////////////////////////////

/* READ COMMANDS */
#define DB_CONTINUOUS_ARRAY_READ       	0xE8		/* Continuous array read */
#define DB_BURST_ARRAY_READ            	0xE8		/* Burst array read */
#define DB_PAGE_READ                   	0xD2		/* Main memory page read */
#define DB_BUF1_READ                   	0xD4		/* Buffer 1 read */
#define DB_BUF2_READ                   	0xD6		/* Buffer 2 read */
#define DB_STATUS                      	0xD7		/* Status Register */

/* PROGRAM and ERASE COMMANDS */
#define DB_BUF1_WRITE                  	0x84		/* Buffer 1 write */
#define DB_BUF2_WRITE                  	0x87		/* Buffer 2 write */
#define DB_BUF1_PAGE_ERASE_PGM         	0x83		/* Buffer 1 to main memory page program with built-In erase */
#define DB_BUF1_PAGE_ERASE_FASTPGM     	0x93		/* Buffer 1 to main memory page program with built-In erase, Fast program */
#define DB_BUF2_PAGE_ERASE_PGM         	0x86		/* Buffer 2 to main memory page program with built-In erase */
#define DB_BUF2_PAGE_ERASE_FASTPGM     	0x96		/* Buffer 1 to main memory page program with built-In erase, Fast program */
#define DB_BUF1_PAGE_PGM               	0x88		/* Buffer 1 to main memory page program without built-In erase */
#define DB_BUF1_PAGE_FASTPGM           	0x98		/* Buffer 1 to main memory page program without built-In erase, Fast program*/
#define DB_BUF2_PAGE_PGM               	0x89		/* Buffer 2 to main memory page program without built-In erase */
#define DB_BUF2_PAGE_FASTPGM           	0x99		/* Buffer 1 to main memory page program without built-In erase, Fast program*/
#define DB_PAGE_ERASE                  	0x81		/* Page Erase */
#define DB_BLOCK_ERASE                 	0x50		/* Block Erase */
#define DB_PAGE_PGM_BUF1               	0x82		/* Main memory page through buffer 1 */
#define DB_PAGE_FASTPGM_BUF1           	0x92		/* Main memory page through buffer 1, Fast program */
#define DB_PAGE_PGM_BUF2               	0x85		/* Main memory page through buffer 2 */
#define DB_PAGE_FastPGM_BUF2           	0x95		/* Main memory page through buffer 2, Fast program */

/* ADDITIONAL COMMANDS */
#define DB_PAGE_2_BUF1_TRF             	0x53		/* Main memory page to buffer 1 transfert */
#define DB_PAGE_2_BUF2_TRF             	0x55		/* Main memory page to buffer 2 transfert */
#define DB_PAGE_2_BUF1_CMP             	0x60		/* Main memory page to buffer 1 compare */
#define DB_PAGE_2_BUF2_CMP             	0x61		/* Main memory page to buffer 2 compare */
#define DB_AUTO_PAGE_PGM_BUF1          	0x58		/* Auto page rewrite throught buffer 1 */
#define DB_AUTO_PAGE_PGM_BUF2          	0x59		/* Auto page rewrite throught buffer 2 */

/////////////////////////////////////////////////////////////////////////////////////////////////////
// DataFlash Structures 
/////////////////////////////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------*/
/* DataFlash Descriptor Structure Definition   */
/*---------------------------------------------*/
typedef struct _AT91S_DataflashDesc
{
	unsigned char			*tx_cmd_pt;
	unsigned int			tx_cmd_size;
	unsigned char			*rx_cmd_pt;
	unsigned int			rx_cmd_size;
	unsigned char			*tx_data_pt;
	unsigned int			tx_data_size;
	unsigned char			*rx_data_pt;
	unsigned int			rx_data_size;
	volatile unsigned char	state;
	volatile unsigned char	DataFlash_state;
	unsigned char			command[8];
} AT91S_DataflashDesc, *AT91PS_DataflashDesc;

/*---------------------------------------------*/
/*DataFlash device definition structure        */
/*---------------------------------------------*/
typedef struct _AT91S_Dataflash {
	int		pages_number;	//* dataflash page number
	int		pages_size;		//* dataflash page size
	int		page_offset;	//* page offset in command
	int		byte_mask;		//* byte mask in command
} AT91S_Dataflash, *AT91PS_Dataflash;

/////////////////////////////////////////////////////////////////////////////////////////////////////

#endif