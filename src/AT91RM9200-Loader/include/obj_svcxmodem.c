//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : obj_svcxmodem.c
//* Object              : Send and receive funtions for USB device peripheral
//*
//* 1.0 20/06/01 	    : ODi Creation
//*----------------------------------------------------------------------------
#include "services/obj_buffer.h"
#include "services/obj_svcxmodem.h"
#include "periph/usart2/lib_usart2.h"

#define CRC16POLY   0x1021              /* CRC 16  polynom */

//*----------------------------------------------------------------------------
//* Function Name       : XmCheckHeader
//* Object              : Compute the CRC
//* Input Parameters    : <ptr>  char pointer
//*                       <count> nb of char
//* Output Parameters   : crc value
//*----------------------------------------------------------------------------
char AT91F_SvcXmodemCheckHeader(unsigned char currentPacket, char *packet)
{

	// Check that bloc number and bloc checksum are OK
	if ( (packet[0] + packet[1]) != 0xff )
		return 0;

	// Check that this is a sequential block number
	if ( packet[0] != currentPacket )
		return 0;

	return 1;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcXmodemCheckCrc
//* Object              : Compute the CRC
//* Input Parameters    : <ptr>  char pointer
//*                       <count> nb of char
//* Output Parameters   : crc value
//*----------------------------------------------------------------------------
unsigned short AT91F_SvcXmodemGetCrc(char *ptr, unsigned int count)
{

    unsigned short crc, cmpt;

    crc = 0;
    //* For  all char
    while (count--)
    {

        crc = crc ^ (int) *ptr++ << 8;
        //* For All bit
        for (cmpt = 0; cmpt < 8; cmpt++)
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ CRC16POLY;
            else
                crc = crc << 1;
        }//* end bit
    }//* Frame end

    return (crc & 0xFFFF);
}



//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcXmodemCheckData
//* Object              : check the data frame, increment the address if OK
//* Input Parameters    : pointer to XmData
//* Output Parameters   : 1 OK, 0 not OK
//*----------------------------------------------------------------------------
char AT91F_SvcXmodemCheckData(AT91PS_SvcXmodem pSvcXmodem)
{
	unsigned short crc;
	char         *pData   = pSvcXmodem->pData;
	unsigned int dataSize = pSvcXmodem->dataSize;

	crc = pSvcXmodem->GetCrc(pData, dataSize);

	if ( (pData[dataSize]   == (unsigned char)(crc >> 8)) &&
	     (pData[dataSize+1] == (unsigned char)(crc)) )
	{
		pSvcXmodem->pData += dataSize;
		return 1;
	}
	else
		return 0;
}


//*----------------------------------------------------------------------------
//* Function Name       : OpenUdpService()
//* Object              :
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91PS_SvcComm AT91F_OpenSvcXmodem(
	AT91PS_SvcXmodem pSvcXmodem,
	AT91PS_USART2    pUsart,
	AT91PS_CtlTempo  pCtlTempo)
{
	AT91PS_SvcComm pSvcComm = &(pSvcXmodem->parent);

	// Init AT91S_CommSvc base structure

	// Init AT91PS_SvcComm methods
	pSvcComm->Reset        = (AT91PF_SvcCommReset)   0;
	pSvcComm->StartTx      = AT91F_SvcXmodemStartTx;
	pSvcComm->StartRx      = AT91F_SvcXmodemStartRx;
	pSvcComm->StopTx       = AT91F_SvcXmodemStopTx;
	pSvcComm->StopRx       = AT91F_SvcXmodemStopRx;
	pSvcComm->TxReady      = AT91F_SvcXmodemTxReady;
	pSvcComm->RxReady      = AT91F_SvcXmodemRxReady;

	// Init AT91PS_SvcComm data
	pSvcComm->pChild       = (void *) pSvcXmodem;
	pSvcComm->pBuffer      = (AT91PS_Buffer) 0;

	// Init AT91PS_SvcXmodem methods
	pSvcXmodem->ReadHandler  = AT91F_SvcXmodemReadHandler;
	pSvcXmodem->WriteHandler = AT91F_SvcXmodemWriteHandler;
	pSvcXmodem->GetCrc       = AT91F_SvcXmodemGetCrc;
	pSvcXmodem->CheckHeader  = AT91F_SvcXmodemCheckHeader;
	pSvcXmodem->CheckData    = AT91F_SvcXmodemCheckData;

	// Init AT91PS_SvcXmodem data
	pSvcXmodem->pUsart = pUsart;

	// Attach the tempo to a tempo controler
	pCtlTempo->CtlTempoCreate(pCtlTempo, &(pSvcXmodem->tempo));

	return pSvcComm;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcXmodemRead()
//* Object              : Receive an XMODEM file with the USART, update the
//*                       linked buffer structure
//* Input Parameters    : AT91PS_SvcXmodem, AT91PS_USART2->US_CSR
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcXmodemReadHandler(AT91PS_SvcXmodem pSvcXmodem, unsigned int csr)
{
	AT91PS_USART2      pUsart  = pSvcXmodem->pUsart;
	AT91PS_Buffer      pBuffer = (AT91PS_Buffer) pSvcXmodem->parent.pBuffer;

	char *pData;
	unsigned int bufSize;
	char c;

	if ( (csr & US_RXRDY) ) {
		// Stop sending 'C'
		pSvcXmodem->tempo.Stop(&(pSvcXmodem->tempo));
		pSvcXmodem->isPacketDesc = 1;
		pBuffer->GetRdBuffer(pBuffer, &pData, &bufSize);
		pSvcXmodem->pData    = pData;

		c = at91_usart2_getChar(pUsart);
		switch (c) {
			case AT91C_XMODEM_STX:
				pSvcXmodem->dataSize = AT91C_XMODEM_DATA_SIZE_STX;
				at91_usart2_receive_frame (
					pUsart,
					pSvcXmodem->packetDesc, AT91C_XMODEM_PACKET_SIZE,
					pData, AT91C_XMODEM_DATA_SIZE_STX + AT91C_XMODEM_CRC_SIZE);
				break;
			case AT91C_XMODEM_SOH:
				pSvcXmodem->dataSize = AT91C_XMODEM_DATA_SIZE_SOH;
				at91_usart2_receive_frame (
					pUsart,
					pSvcXmodem->packetDesc, AT91C_XMODEM_PACKET_SIZE,
					pData, AT91C_XMODEM_DATA_SIZE_SOH + AT91C_XMODEM_CRC_SIZE);
				break;
			case AT91C_XMODEM_EOT:
				at91_usart2_sendChar(pUsart, AT91C_XMODEM_ACK);
				at91_usart2_usidr(pUsart, US_RXRDY | US_ENDRX | US_RXBUFF);								
				if ( (pSvcXmodem->packetId == 1) || bufSize)
					pBuffer->FillRdBuffer(pBuffer, bufSize);
				pSvcXmodem->eot = 1;
				return AT91C_COMMSVC_SUCCESS;
			default:
				at91_usart2_receive_frame (
					pUsart,
					pData, AT91C_XMODEM_PACKET_SIZE + AT91C_XMODEM_DATA_SIZE_SOH + AT91C_XMODEM_CRC_SIZE,
					0, 0);
				at91_usart2_sendChar(pUsart, AT91C_XMODEM_ACK);
				break;
		}

		at91_usart2_usidr(pUsart, US_RXRDY);
		at91_usart2_usier(pUsart, US_ENDRX);
	}

	else if ( (csr & US_ENDRX) || (csr & US_RXBUFF) ) {
		if (pSvcXmodem->isPacketDesc) {
			pSvcXmodem->packetStatus = pSvcXmodem->CheckHeader(pSvcXmodem->packetId, pSvcXmodem->packetDesc);
			pSvcXmodem->isPacketDesc = 0;
			at91_usart2_usidr(pUsart, US_ENDRX);
			at91_usart2_usier(pUsart, US_RXBUFF);

		}
		else {
			if (pSvcXmodem->packetStatus && pSvcXmodem->CheckData(pSvcXmodem)) {
				pBuffer->FillRdBuffer(pBuffer, pSvcXmodem->dataSize);
				at91_usart2_sendChar(pUsart, AT91C_XMODEM_ACK);
				++(pSvcXmodem->packetId);
			}
			else
				at91_usart2_sendChar(pUsart, AT91C_XMODEM_NAK);

			at91_usart2_usidr(pUsart, US_ENDRX | US_RXBUFF);
			at91_usart2_usier(pUsart, US_RXRDY);
		}
	}
	return AT91C_COMMSVC_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcXmodemWrite()
//* Object              : Receive an XMODEM file with the USART, update the
//*                       linked buffer structure
//* Input Parameters    : AT91PS_SvcXmodem, AT91PS_USART2->US_CSR
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcXmodemWriteHandler(AT91PS_SvcXmodem pSvcXmodem, unsigned int csr)
{
	AT91PS_USART2      pUsart  = pSvcXmodem->pUsart;
	AT91PS_Buffer      pBuffer = (AT91PS_Buffer) pSvcXmodem->parent.pBuffer;

	char const *pData;
	unsigned int bufSize;
	unsigned short crc;

	if ( (csr & US_RXRDY) ) {
		pBuffer->GetWrBuffer(pBuffer, &pData, &bufSize);
		switch ( at91_usart2_getChar(pUsart) ) {
		case 'C':
			pSvcXmodem->packetId = 1; // Current packet
			if (bufSize == 0)
				pBuffer->EmptyWrBuffer(pBuffer, 0);
			break;
		case AT91C_XMODEM_ACK:
			// Previous block has been received correctly
			if ( !(pSvcXmodem->eot) ) {
				pBuffer->EmptyWrBuffer(pBuffer, AT91C_XMODEM_DATA_SIZE_SOH);
				bufSize = 0;
			}
			++(pSvcXmodem->packetId);
			break;
		case AT91C_XMODEM_NAK:
			// Previous block must be resent
			break;
		default:
			break;
		}
		if (bufSize == 0) {
			at91_usart2_usidr(pUsart, US_ENDRX | US_ENDTX | US_TXRDY);
			at91_usart2_sendChar(pUsart, AT91C_XMODEM_EOT);
			pSvcXmodem->eot = 1;
		}
		else {
			pSvcXmodem->pData = (char *) pData;
			pSvcXmodem->eot           = 0;

			at91_usart2_sendChar(pUsart, AT91C_XMODEM_SOH);
			at91_usart2_usier(pUsart, US_TXRDY);
		}
	}
	else if ( (csr & US_TXRDY) ) {
		pSvcXmodem->packetDesc[0] = pSvcXmodem->packetId;
		pSvcXmodem->packetDesc[1] = ~(pSvcXmodem->packetId);
		at91_usart2_send_frame (
			pUsart,
			pSvcXmodem->packetDesc, AT91C_XMODEM_PACKET_SIZE,
			pSvcXmodem->pData,      AT91C_XMODEM_DATA_SIZE_SOH);
		at91_usart2_usidr(pUsart, US_TXRDY);
		at91_usart2_usier(pUsart, US_ENDTX);
	}
	else if ( (csr & US_ENDTX) ) {
		crc = pSvcXmodem->GetCrc(pSvcXmodem->pData, AT91C_XMODEM_DATA_SIZE_SOH);
		crc = ((crc & 0xff00) >> 8) | ((crc & 0x00ff) << 8 );
		at91_usart2_send_frame (
			pUsart,
			(char *) &crc, AT91C_XMODEM_CRC_SIZE,
			(char *) 0,0);
		at91_usart2_usidr(pUsart, US_ENDTX);
	}
	return AT91C_COMMSVC_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcUdpStartTx()
//* Object              :
//* Input Parameters    : AT91PS_SvcComm
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcXmodemStartTx(AT91PS_SvcComm pSvcComm)
{
	AT91PS_SvcXmodem   pSvcXmodem = (AT91PS_SvcXmodem) pSvcComm->pChild;
	AT91PS_USART2      pUsart     = pSvcXmodem->pUsart;

	pSvcXmodem->Handler = pSvcXmodem->WriteHandler;
	at91_usart2_usier(pUsart, US_RXRDY);

	return AT91C_COMMSVC_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcUdpStoptTx()
//* Object              :
//* Input Parameters    : AT91PS_SvcComm
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcXmodemStopTx(AT91PS_SvcComm pSvcComm)
{

	return AT91C_COMMSVC_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcXmodemSendC()
//* Object              :
//* Input Parameters    : AT91PS_SvcComm
//* Output Parameters   : none
//*----------------------------------------------------------------------------
void AT91F_SvcXmodemSendC(AT91S_TempoStatus status, void *pVoid)
{
		AT91PS_USART2 pUsart = (AT91PS_USART2) pVoid;

		at91_usart2_sendChar(pUsart, 'C');
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcXmodemStartRx()
//* Object              :
//* Input Parameters    : AT91PS_SvcComm
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcXmodemStartRx(AT91PS_SvcComm pSvcComm)
{
	AT91PS_SvcXmodem   pSvcXmodem = (AT91PS_SvcXmodem) pSvcComm->pChild;
	AT91PS_USART2      pUsart     = pSvcXmodem->pUsart;

	pSvcXmodem->Handler = pSvcXmodem->ReadHandler;
	pSvcXmodem->packetId     = 1; // Current packet
	pSvcXmodem->eot          = 0; // end of transmition
	pSvcXmodem->isPacketDesc = 0;
	at91_usart2_usier(pUsart, US_RXRDY);
	at91_usart2_usidr(pUsart, US_ENDRX | US_ENDTX | US_TXRDY);
	
	pSvcXmodem->tempo.Start(&(pSvcXmodem->tempo), 1000, 1000, AT91F_SvcXmodemSendC, pUsart);
//	pSvcXmodem->tempo.Start(&(pSvcXmodem->tempo), 5, 5, AT91F_SvcXmodemSendC, pUsart);

	return AT91C_COMMSVC_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcUdpStoptRx()
//* Object              :
//* Input Parameters    : AT91PS_SvcComm
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcXmodemStopRx(AT91PS_SvcComm pSvcComm)
{

	return AT91C_COMMSVC_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_UdpTxReady()
//* Object              : Return the status of the USB
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
char AT91F_SvcXmodemTxReady(AT91PS_SvcComm pSvcComm)
{
	AT91PS_SvcXmodem   pSvcXmodem = (AT91PS_SvcXmodem) pSvcComm->pChild;
	AT91PS_USART2      pUsart     = pSvcXmodem->pUsart;

	return (AT91F_US_CSR(pUsart) & US_TXRDY);

}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_UdpRxReady()
//* Object              : Return the status of the USB
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
char AT91F_SvcXmodemRxReady(AT91PS_SvcComm pSvcComm)
{
	AT91PS_SvcXmodem   pSvcXmodem = (AT91PS_SvcXmodem) pSvcComm->pChild;
	AT91PS_USART2      pUsart     = pSvcXmodem->pUsart;

	return (AT91F_US_CSR(pUsart) & US_RXRDY);

}