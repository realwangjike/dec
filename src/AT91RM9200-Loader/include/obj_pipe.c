//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : DfuPipe.c
//* Object              : Application pipe interface
//*
//* 1.0 23/03/01 	    : HI Creation
//* 1.0 20/06/01 	    : ODi Review for DFU
//*----------------------------------------------------------------------------
#include "lib_debug.h"
#include "services/obj_pipe.h"

#ifdef DBG_DRV_PIPE
#include <assert.h>

const char *PipeErrMsg[] = {
	"Pipe Success",
	"Pipe Open failed",
	"Pipe Write failed",
	"Pipe Read failed",
	"Pipe Abort failed",
	"Pipe Reset failed",
	"Pipe Unknown error"
};

char const *AT91F_PipeGetError(AT91S_PipeStatus msgId)
{
	switch (msgId & AT91C_PIPE_ERROR) {
	case AT91C_PIPE_SUCCESS:      return (PipeErrMsg[0]);
	case AT91C_PIPE_OPEN_FAILED:  return (PipeErrMsg[1]);
	case AT91C_PIPE_WRITE_FAILED: return (PipeErrMsg[2]);
	case AT91C_PIPE_READ_FAILED:  return (PipeErrMsg[3]);
	case AT91C_PIPE_ABORT_FAILED: return (PipeErrMsg[4]);
	case AT91C_PIPE_RESET_FAILED: return (PipeErrMsg[5]);
	default:                      return (PipeErrMsg[6]);
	}
}
#endif

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_PipeOpen()
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91PS_Pipe AT91F_OpenPipe(
	AT91PS_Pipe    pPipe,
	AT91PS_SvcComm pSvcComm,
	AT91PS_Buffer  pBuffer)
{
	AT91S_BufferStatus status;
#ifdef DBG_DRV_PIPE
	assert(pBuffer);
	assert(pSvcComm);
	assert(pBuffer->RstRdBuffer);
	assert(pBuffer->RstWrBuffer);
#endif

	// Init pipe members
	pPipe->pSvcComm   = pSvcComm;
	pPipe->pBuffer    = pBuffer;
	// Init the API with default functions
	pPipe->Write      = AT91F_PipeWrite;
	pPipe->Read       = AT91F_PipeRead;
	pPipe->AbortWrite = AT91F_PipeAbortWrite;
	pPipe->AbortRead  = AT91F_PipeAbortRead;
	pPipe->Reset      = AT91F_PipeReset;
	pPipe->IsWritten  = AT91F_PipeMsgWritten;
	pPipe->IsReceived = AT91F_PipeMsgReceived;

	pPipe->WriteCallback = (AT91PF_PipeWriteCallBack) 0;
	pPipe->ReadCallback  = (AT91PF_PipeReadCallBack) 0;
	pPipe->pPrivateReadData  = (void *) 0;
	pPipe->pPrivateWriteData = (void *) 0;

	// Update pSvcComm fields
	pSvcComm->pBuffer = pBuffer;

	// Init pBuffer members
	pBuffer->pPipe = pPipe;
	// Reset the read and write buffer
	pBuffer->RstRdBuffer(pBuffer);
	if ( status ) return ((AT91PS_Pipe) 0);
	status = pBuffer->RstWrBuffer(pBuffer);
	if ( status ) return ((AT91PS_Pipe) 0);

	return pPipe;
}


//*----------------------------------------------------------------------------
//* Function Name       : AT91F_PipeWrite()
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_PipeStatus AT91F_PipeWrite(
	AT91PS_Pipe pPipe,
	char const *pVoid,
	unsigned int size,
	AT91PF_PipeReadCallBack callback,
	void *privateData)
{
	AT91PS_SvcComm   pSvcComm = pPipe->pSvcComm;
	AT91PS_Buffer    pBuffer  = pPipe->pBuffer;
	AT91S_PipeStatus status   = AT91C_PIPE_SUCCESS;

#ifdef DBG_DRV_PIPE
	assert(pBuffer);
	assert(pBuffer->SetWrBuffer);
	assert(pSvcComm);
	assert(pSvcComm->TxReady);
#endif

	// Initialize callback
	pPipe->pPrivateWriteData = privateData;
	pPipe->WriteCallback     = callback;

	// Check the periph is ready to accept transfer
	if ( !(pSvcComm->TxReady(pSvcComm)) )
		return (AT91C_PIPE_WRITE_FAILED);

	// Initialize the buffer
	status = pBuffer->SetWrBuffer(pBuffer, pVoid, size);
	if ( status )
		return (AT91C_PIPE_WRITE_FAILED | status);

	// Initialize the peripheral
	if ( pSvcComm->StartTx ) {
		status = pSvcComm->StartTx(pSvcComm);
		if ( status )
			return (AT91C_PIPE_WRITE_FAILED | status);
	}

	return AT91C_PIPE_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_PipeRead()
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_PipeStatus AT91F_PipeRead(
	AT91PS_Pipe        pPipe,
	char               *pVoid,
	unsigned int       size,
	AT91PF_PipeReadCallBack callback,
	void *privateData)
{
	AT91PS_SvcComm pSvcComm = pPipe->pSvcComm;
	AT91PS_Buffer  pBuffer  = pPipe->pBuffer;
	AT91S_PipeStatus status = AT91C_PIPE_SUCCESS;

#ifdef DBG_DRV_PIPE
	assert(pBuffer);
	assert(pBuffer->SetRdBuffer);
	assert(pSvcComm);
#endif

	// Initialize the callback
	pPipe->pPrivateReadData = privateData;
	pPipe->ReadCallback    = callback;

	// Initialize the buffer
	status = pBuffer->SetRdBuffer(pBuffer, pVoid, size);
	if ( status )
		return (AT91C_PIPE_READ_FAILED | status);

	// Initialize the peripheral
	if ( pSvcComm->StartRx) {
		status = pSvcComm->StartRx(pSvcComm);
		if ( status )
			return (AT91C_PIPE_READ_FAILED | status);
	}


	return AT91C_PIPE_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_PipeAbortWrite()
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_PipeStatus AT91F_PipeAbortWrite(
	AT91PS_Pipe pPipe)
{
	AT91PS_SvcComm pSvcComm = pPipe->pSvcComm;
	AT91PS_Buffer  pBuffer  = pPipe->pBuffer;
	AT91S_PipeStatus status = AT91C_PIPE_SUCCESS;

#ifdef DBG_DRV_PIPE
	assert(pBuffer);
	assert(pBuffer->RstWrBuffer);
	assert(pSvcComm);
#endif

	// Reset the write Buffer
	status = pBuffer->RstWrBuffer(pBuffer);
	if ( status )
		return (AT91C_PIPE_ABORT_FAILED | status);

	// Reset the peripheral
	if ( pSvcComm->StopTx) {
		status = pSvcComm->StopTx(pSvcComm);
		if ( !status )
			return (AT91C_PIPE_ABORT_FAILED | status);
	}

	// Invoke the callback
	if (pPipe->WriteCallback)
		pPipe->WriteCallback(AT91C_PIPE_WRITE_ABORTED, pPipe->pPrivateWriteData);

	return AT91C_PIPE_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_PipeAbortRead()
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_PipeStatus AT91F_PipeAbortRead(
	AT91PS_Pipe pPipe)
{
	AT91PS_SvcComm pSvcComm = pPipe->pSvcComm;
	AT91PS_Buffer  pBuffer  = pPipe->pBuffer;
	AT91S_PipeStatus status = AT91C_PIPE_SUCCESS;

#ifdef DBG_DRV_PIPE
	assert(pBuffer);
	assert(pBuffer->RstRdBuffer);
	assert(pSvcComm);
#endif

	// Reset the read Buffer
	status = pBuffer->RstRdBuffer(pBuffer);
	if ( status )
		return (AT91C_PIPE_ABORT_FAILED | status);

	// Reset the peripheral
	if ( pSvcComm->StopRx) {
		status = pSvcComm->StopRx(pSvcComm);
		if ( status )
			return (AT91C_PIPE_ABORT_FAILED | status);
	}

	// Invoke the callback
	if (pPipe->ReadCallback)
		pPipe->ReadCallback(AT91C_PIPE_READ_ABORTED, pPipe->pPrivateReadData);

	return AT91C_PIPE_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_PipeReset()
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_PipeStatus AT91F_PipeReset(
	AT91PS_Pipe pPipe)
{
	AT91PS_SvcComm pSvcComm = pPipe->pSvcComm;
	AT91PS_Buffer  pBuffer  = pPipe->pBuffer;
	AT91S_PipeStatus status = AT91C_PIPE_SUCCESS;

#ifdef DBG_DRV_PIPE
	assert(pBuffer);
	assert(pSvcComm);
#endif

	// Reset the read and write buffer
	if ( pBuffer->RstRdBuffer) {
	     status = pBuffer->RstRdBuffer(pBuffer);
	     if ( status )
			return (AT91C_PIPE_RESET_FAILED | status);
	}
	if ( pBuffer->RstWrBuffer) {
		status = pBuffer->RstWrBuffer(pBuffer);
		if ( status )
			return (AT91C_PIPE_RESET_FAILED | status);
	}

	// Reset the peripheral
	if ( pSvcComm->Reset) {
		status = pSvcComm->Reset(pSvcComm);
		if ( status )
			return (AT91C_PIPE_RESET_FAILED | status);
	}

	return AT91C_PIPE_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_PipeMsgWritten()
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
char AT91F_PipeMsgWritten(
	AT91PS_Pipe pPipe,
	char const *pVoid)
{
	AT91PS_Buffer  pBuffer  = pPipe->pBuffer;
	return (pBuffer->MsgWritten(pBuffer, pVoid));

}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_PipeMsgReceived()
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
char AT91F_PipeMsgReceived(
	AT91PS_Pipe pPipe,
	char const  *pVoid)
{
	AT91PS_Buffer  pBuffer  = pPipe->pBuffer;
	return (pBuffer->MsgRead(pBuffer, pVoid));
}
