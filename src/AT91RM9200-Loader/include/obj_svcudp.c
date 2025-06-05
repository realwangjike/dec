//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : drv_usb.c
//* Object              : Send and receive funtions for USB device peripheral
//*
//* 1.0 20/06/01 	    : ODi Creation
//*----------------------------------------------------------------------------
#include "services/obj_buffer.h"
#include "services/obj_svcudp.h"
#include "periph/udp/lib_udp.h"

#ifdef DBG_OBJ_UDP
#include <assert.h>
const char *UdpErrMsg[] = {
	"(AT91_SvcUdp) Receive has failed",
	"(AT91_SvcUdp) Read buffer not ready"
	"(AT91_SvcUdp) Receive not allowed",
	"(AT91_SvcUdp) Write failed",
	"(AT91_SvcUdp) Write not ready",
	"(AT91_SvcUdp) Write aborted",
	"(AT91_SvcUdp) Unknown message"
};
#endif
//*----------------------------------------------------------------------------
//* Function Name       : OpenUdpService()
//* Object              :
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91PS_SvcComm AT91F_OpenSvcUdp(
	AT91PS_SvcUdp pSvcUdp,
	AT91PS_UDP   pUdp,
	char         epId,
	unsigned int epSize)
{
	AT91PS_SvcComm pSvcComm = &(pSvcUdp->parent);

#ifdef DBG_DRV_UDP
	assert(pSvcUdp);
	assert(pUdp);
	assert(pSvcComm);
#endif

	// Init AT91S_CommSvc base structure

	// Init methods
	pSvcComm->Reset        = (AT91PF_SvcCommReset)   0;
	pSvcComm->StartTx      = AT91F_SvcUdpStartTx;
	pSvcComm->StartRx      = AT91F_SvcUdpStartRx;
	pSvcComm->StopTx       = AT91F_SvcUdpStopTx;
	pSvcComm->StopRx       = AT91F_SvcUdpStopRx;
	pSvcComm->TxReady      = AT91F_SvcUdpTxReady;
	pSvcComm->RxReady      = AT91F_SvcUdpRxReady;

	// Init data
	pSvcComm->pChild       = (void *) pSvcUdp;
	pSvcComm->pBuffer      = (AT91PS_Buffer) 0;

	// Init AT91S_UdpEp structure
	pSvcUdp->HandleRx   = AT91F_SvcUdpControlHandleRx;
	pSvcUdp->HandleTx   = AT91F_SvcUdpControlHandleTx;
	pSvcUdp->pUdp       = pUdp;
	pSvcUdp->epId       = epId;
	pSvcUdp->epSize     = epSize;
	pSvcUdp->bank       = UDP_RCV_DATA_BANK0;
	pSvcUdp->operation  = AT91C_SVCUDP_CTL_RD;

	return pSvcComm;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcUdpStartTx()
//* Object              : Send or receive a payload with the USB, update the
//*                       linked buffer structure
//* Input Parameters    : AT91PS_SvcComm
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcUdpStartTx(AT91PS_SvcComm pSvcComm)
{
	AT91PS_Buffer  pBuffer  = (AT91PS_Buffer) pSvcComm->pBuffer;
	AT91S_BufferStatus status = AT91C_COMMSVC_SUCCESS;
	char const *pWrData;
	unsigned int bufSize;
	AT91PS_SvcUdp pSvcUdp  = (AT91PS_SvcUdp) pSvcComm->pChild;

	pSvcUdp->operation = AT91C_SVCUDP_CTL_RD;
	// Get how many bytes to send now
	status = pBuffer->GetWrBuffer(pBuffer, &pWrData, &bufSize);
	if (status )
		status = (AT91C_SVCUDP_WR_FAILED | status);
	// Send the first packet
	if (pSvcUdp->epSize < bufSize)
		AT91F_UdpEpWrite(pSvcUdp->pUdp, pSvcUdp->epId, pWrData, pSvcUdp->epSize);
	else if (bufSize)
		AT91F_UdpEpWrite(pSvcUdp->pUdp, pSvcUdp->epId, pWrData, bufSize);
	// Notify the end of write USB device peripheral
	AT91F_UdpEpEow(pSvcUdp->pUdp, pSvcUdp->epId);

	return AT91C_COMMSVC_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcUdpStoptTx()
//* Object              :
//* Input Parameters    : AT91PS_SvcComm
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcUdpStopTx(AT91PS_SvcComm pSvcComm)
{
	AT91PS_SvcUdp pSvcUdp = (AT91PS_SvcUdp) pSvcComm->pChild;

	AT91F_UdpEpStall(pSvcUdp->pUdp, pSvcUdp->epId);
	return AT91C_COMMSVC_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcUdpStartRx()
//* Object              :
//* Input Parameters    : AT91PS_SvcComm
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcUdpStartRx(AT91PS_SvcComm pSvcComm)
{
	AT91PS_SvcUdp pSvcUdp  = (AT91PS_SvcUdp) pSvcComm->pChild;

	pSvcUdp->operation = AT91C_SVCUDP_CTL_WR;

	return AT91C_COMMSVC_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcUdpStoptRx()
//* Object              :
//* Input Parameters    : AT91PS_SvcComm
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcUdpStopRx(AT91PS_SvcComm pSvcComm)
{
	AT91PS_SvcUdp pSvcUdp = (AT91PS_SvcUdp) pSvcComm->pChild;

	AT91F_UdpEpStall(pSvcUdp->pUdp, pSvcUdp->epId);

	return AT91C_COMMSVC_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcUdpControlHandleRx()
//* Object              : Send or receive a payload with the USB, update the
//*                       linked buffer structure
//* Input Parameters    : AT91PS_SvcComm
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcUdpControlHandleRx(AT91PS_SvcUdp pSvcUdp)
{
	AT91PS_SvcComm pService = (AT91PS_SvcComm) &(pSvcUdp->parent);
	AT91PS_Buffer  pBuffer  = (AT91PS_Buffer) pService->pBuffer;
	AT91S_BufferStatus status = AT91C_COMMSVC_SUCCESS;
	char *pRdData;
	char const *pWrData;
	unsigned int bufSize, bytesRcv;

	if (pSvcUdp->operation == AT91C_SVCUDP_CTL_RD) {
		// This is the STATUS OUT stage, it stops the transmition
		AT91F_UdpEpEor(pSvcUdp->pUdp, pSvcUdp->epId, UDP_RCV_DATA_BANK0);
		status = pBuffer->GetWrBuffer(pBuffer, &pWrData, &bufSize);
		if ( status )
			status = (AT91C_SVCUDP_WR_FAILED | status);

		// Free the remaining buffer
		if (bufSize) {
			status = pBuffer->EmptyWrBuffer(pBuffer, bufSize);
			status = ( status ) ? (AT91C_SVCUDP_WR_FAILED | status) : AT91C_SVCUDP_WR_ABORTED;
		}

		return status;
	}
	else {
		status = pBuffer->GetRdBuffer(pBuffer, &pRdData, &bufSize);
		if ( status )
			status = (AT91C_SVCUDP_RD_FAILED | status);

		// Determine how many bytes have been received
		bytesRcv = AT91F_UdpEpNbBytesRcv(pSvcUdp->pUdp, pSvcUdp->epId);

		// An unexpected packet length has been received, STALL it.
		if (bytesRcv > bufSize) {
			AT91F_UdpEpEor(pSvcUdp->pUdp, pSvcUdp->epId, UDP_RCV_DATA_BANK0);
			AT91F_UdpEpStall(pSvcUdp->pUdp, pSvcUdp->epId);
			status = AT91C_SVCUDP_RD_BUFFER_NOT_RDY;
		}
		// Accept the packet
		else {
			// Read the USB DPR
			AT91F_UdpEpRead(pSvcUdp->pUdp, pSvcUdp->epId, pRdData, bytesRcv);
			AT91F_UdpEpEor(pSvcUdp->pUdp, pSvcUdp->epId, UDP_RCV_DATA_BANK0);

			// Update the buffer
			status = pBuffer->FillRdBuffer(pBuffer, bytesRcv);
			if ( status )
				status = (AT91C_SVCUDP_RD_FAILED | status);
		}

		// Notify the call back if the buffer is full


	}
	return status;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SvcUdpControlHandleTx()
//* Object              : Send or receive a payload with the USB, update the
//*                       linked buffer structure
//* Input Parameters    : AT91PS_SvcComm
//* Output Parameters   : none
//*----------------------------------------------------------------------------
AT91S_SvcCommStatus AT91F_SvcUdpControlHandleTx(AT91PS_SvcUdp pSvcUdp)
{
	AT91PS_SvcComm pService = (AT91PS_SvcComm) &(pSvcUdp->parent);
	AT91PS_Buffer  pBuffer  = (AT91PS_Buffer) pService->pBuffer;
	AT91S_BufferStatus status = AT91C_COMMSVC_SUCCESS;
	char const *pWrData;
	unsigned int bufSize;

	if (pSvcUdp->operation == AT91C_SVCUDP_CTL_RD) {
		// Free the buffer sent and ACKed
		status = pBuffer->EmptyWrBuffer(pBuffer, pSvcUdp->epSize);
		status = ( status ) ? (AT91C_SVCUDP_WR_FAILED | status) : AT91C_SVCUDP_WR_ABORTED;
		// Get how many bytes to send now
		status = pBuffer->GetWrBuffer(pBuffer, &pWrData, &bufSize);
		if (status )
			status = (AT91C_SVCUDP_WR_FAILED | status);
		// If required, send another packet
		if (bufSize) {
			if (pSvcUdp->epSize < bufSize)
				AT91F_UdpEpWrite(pSvcUdp->pUdp, pSvcUdp->epId, pWrData, pSvcUdp->epSize);
			else
				AT91F_UdpEpWrite(pSvcUdp->pUdp, pSvcUdp->epId, pWrData, bufSize);
			// Notify the end of write USB device peripheral
			AT91F_UdpEpEow(pSvcUdp->pUdp, pSvcUdp->epId);
		}
	}

	return status;
}



//*----------------------------------------------------------------------------
//* Function Name       : AT91F_UdpTxReady()
//* Object              : Return the status of the USB
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
char AT91F_SvcUdpTxReady(AT91PS_SvcComm pService)
{
	AT91PS_SvcUdp pSvcUdp = (AT91PS_SvcUdp) pService->pChild;

	return (AT91F_UdpEpWok (pSvcUdp->pUdp, pSvcUdp->epId));

}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_UdpRxReady()
//* Object              : Return the status of the USB
//* Input Parameters    :
//* Output Parameters   : none
//*----------------------------------------------------------------------------
char AT91F_SvcUdpRxReady(AT91PS_SvcComm pSvcComm)
{
	AT91PS_SvcUdp pSvcUdp = (AT91PS_SvcUdp) pSvcComm->pChild;
	char status = AT91F_UdpEpRok(pSvcUdp->pUdp, pSvcUdp->epId);

	if ( status == UDP_RCV_DATA_BANK0 )
		pSvcUdp->bank = UDP_RCV_DATA_BANK0;
	else if ( status == UDP_RCV_DATA_BANK1 )
		pSvcUdp->bank = UDP_RCV_DATA_BANK1;
	return status;

}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_UdpGetError()
//* Object              : Return the read error message
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
#ifdef DBG_DRV_PERIPH
char const *AT91F_SvcUdpGetError(AT91S_SvcCommStatus errorNumber)
{

	switch (errorNumber & AT91C_SERVICE_ERROR)
	case AT91C_SVCUDP_RD_FAILED:         return (UdpErrMsg[1]);
	case AT91C_SVCUDP_RD_BUFFER_NOT_RDY: return (UdpErrMsg[2]);
	case AT91C_SVCUDP_RD_NOT_RDY:        return (UdpErrMsg[3]);
	case AT91C_SVCUDP_WR_FAILED:         return (UdpErrMsg[4]);
	case AT91C_SVCUDP_WR_NOT_RDY:        return (UdpErrMsg[5]);
	case AT91C_SVCUDP_WR_ABORTED:        return (UdpErrMsg[6]);
	default:                             return (UdpErrMsg[7]);
	}
}
#endif