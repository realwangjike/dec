//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : eeprom.h
//* Object              :
//*----------------------------------------------------------------------------
#ifndef eeprom_h
#define eeprom_h


#define AT91C_EEPROM_PAGE_SIZE 			32

#define AT91C_ERR_EEPROM_READ			-1
#define AT91C_ERR_EEPROM_WRITE			-2
#define AT91C_ERR_EEPROM_READ_PAGE		-3
#define AT91C_EEPROM_READ_OK			0
#define AT91C_EEPROM_WRITE_OK			0
#define AT91C_EEPROM_READ_PAGE_OK		0
#define AT91C_EEPROM_TIMEOUT_READ		2			/* read timeout in ms */
#define AT91C_EEPROM_TIMEOUT_WRITE		15			/* write timeout in ms */

#define	AT91C_TWI_ERROR   ( AT91C_TWI_NACK | AT91C_TWI_UNRE | AT91C_TWI_OVRE )
#define AT91C_EEPROM_I2C_ADDRESS  	0x00500000


extern int AT91F_TWI_WriteByte (const AT91PS_TWI, int, char );
extern int AT91F_TWI_ReadByte  (const AT91PS_TWI, int, char *);
extern int AT91F_TWI_WritePage (const AT91PS_TWI, int, char *);
extern int AT91F_EEPROM_SeqRead(const AT91PS_TWI, int, char *, int );
extern int AT91F_TWI_ReadPage  (const AT91PS_TWI, int, char *);



#endif