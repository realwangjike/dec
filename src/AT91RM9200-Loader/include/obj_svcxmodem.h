//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : drv_usb.h
//* Object              : Send and receive funtions for USB device peripheral
//*
//* 1.0 20/06/01 	    : ODi Creation
//*----------------------------------------------------------------------------
#ifndef obj_svcxmodem_h
#define obj_svcxmodem_h

#include "periph/usart2/lib_usart2.h"
#include "services/obj_svccomm.h"
#include "services/obj_svctempo.h"

// Constants:

#define AT91C_XMODEM_SOH         0x01         /* Start of Heading for 128 bytes */
#define AT91C_XMODEM_STX         0x02         /* Start of heading for 1024 bytes */
#define AT91C_XMODEM_EOT         0x04         /* End of transmission */
#define AT91C_XMODEM_ACK         0x06         /* Acknowledge */
#define AT91C_XMODEM_NAK         0x15         /* Negative Acknowledge */
#define AT91C_XMODEM_CRCCHR      'C'

#define AT91C_XMODEM_PACKET_SIZE 2                 // packet + packetCRC
#define AT91C_XMODEM_CRC_SIZE    2                 // crcLSB + crcMSB
#define AT91C_XMODEM_DATA_SIZE_SOH    128          // data 128 corresponding to SOH header
#define AT91C_XMODEM_DATA_SIZE_STX    1024         // data 1024 corresponding to STX header


//* ====================================================================================
//* Following structure is used by SPipe to refer to the USB device peripheral endpoint
typedef struct _AT91PS_SvcXmodem {

	// Public Methods:
	AT91S_SvcCommStatus (*Handler) (struct _AT91PS_SvcXmodem *, unsigned int);
	AT91S_SvcCommStatus (*StartTx) (struct _AT91PS_SvcXmodem *, unsigned int);
	AT91S_SvcCommStatus (*StopTx)  (struct _AT91PS_SvcXmodem *, unsigned int);

	// Private Methods:
	AT91S_SvcCommStatus (*ReadHandler)  (struct _AT91PS_SvcXmodem *, unsigned int csr);
	AT91S_SvcCommStatus (*WriteHandler) (struct _AT91PS_SvcXmodem *, unsigned int csr);
	unsigned short      (*GetCrc)       (char *ptr, unsigned int count);
	char                (*CheckHeader)  (unsigned char currentPacket, char *packet);
	char                (*CheckData)    (struct _AT91PS_SvcXmodem *);

	AT91S_SvcComm  parent;      // Base class
	AT91PS_USART2  pUsart;

	AT91S_SvcTempo tempo; // Link to a AT91S_Tempo object

	char          *pData;
	unsigned int  dataSize;        // = XMODEM_DATA_STX or XMODEM_DATA_SOH
	char          packetDesc[AT91C_XMODEM_PACKET_SIZE];
	unsigned char packetId;         // Current packet
	char          packetStatus;
	char          isPacketDesc;
	char          eot;            // end of transmition
} AT91S_SvcXmodem, *AT91PS_SvcXmodem;


typedef AT91PS_SvcComm      (*AT91PF_OpenSvcXmodem) ( AT91PS_SvcXmodem, AT91PS_USART2, AT91PS_CtlTempo);

typedef AT91S_SvcCommStatus (*AT91PF_SvcXmodemHandler) (AT91PS_SvcXmodem pSvcXmodem, unsigned int csr);

typedef unsigned short      (*AT91PF_SvcXmodemGetCrc)      (char *ptr, unsigned int count);
typedef char                (*AT91PF_SvcXmodemCheckHeader) (unsigned char currentPacket, char *packet);
typedef char                (*AT91PF_SvcXmodemCheckData)   (AT91PS_SvcXmodem pSvcXmodem);

//* ====================================================================================

// This function is called by the application.
extern AT91PS_SvcComm AT91F_OpenSvcXmodem( AT91PS_SvcXmodem, AT91PS_USART2, AT91PS_CtlTempo);


extern unsigned short AT91F_SvcXmodemGetCrc     (char *ptr, unsigned int count);
extern char           AT91F_SvcXmodemCheckHeader(unsigned char currentPacket, char *packet);
extern char           AT91F_SvcXmodemCheckData  (AT91PS_SvcXmodem pSvcXmodem);

extern AT91S_SvcCommStatus AT91F_SvcXmodemReadHandler(AT91PS_SvcXmodem pSvcXmodem, unsigned int csr);
extern AT91S_SvcCommStatus AT91F_SvcXmodemWriteHandler(AT91PS_SvcXmodem pSvcXmodem, unsigned int csr);

extern AT91S_SvcCommStatus AT91F_SvcXmodemStartTx(AT91PS_SvcComm pSvcComm);
extern AT91S_SvcCommStatus AT91F_SvcXmodemStopTx(AT91PS_SvcComm pSvcComm);

extern AT91S_SvcCommStatus AT91F_SvcXmodemStartRx(AT91PS_SvcComm pSvcComm);
extern AT91S_SvcCommStatus AT91F_SvcXmodemStopRx(AT91PS_SvcComm pSvcComm);

extern char AT91F_SvcXmodemTxReady(AT91PS_SvcComm pService);
extern char AT91F_SvcXmodemRxReady(AT91PS_SvcComm pSvcComm);
#endif // obj_svcxmodem_h
