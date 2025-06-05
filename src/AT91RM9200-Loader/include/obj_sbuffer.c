//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : obj_sbuffer.c
//* Object              : SimpleBuffer implementation (specialization of SBuffer)
//*
//* 1.0 23/03/01 	    : HI Creation
//* 1.0 20/06/01 	    : ODi Adaptation for DFU
//*----------------------------------------------------------------------------
#include "lib_debug.h"
#include "services\obj_sbuffer.h"
#include "services\obj_pipe.h"

#ifdef DBG_DRV_BUFFER
#include <assert.h>
const char *SBufferErrMsg[] = {
	"Buffer Success",
	"Periph has written more bytes than there is in the buffer",
	"Read buffer full",
	"Pipe Unknown error"
};
#endif

//*----------------------------------------------------------------------------
//* Function Name       : OpenSimpleBuffer()
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91PS_Buffer AT91F_OpenSBuffer(
	AT91PS_SBuffer pSimpleBuffer)
{
	AT91PS_Buffer pBuffer = &(pSimpleBuffer->parent);
#ifdef DBG_DRV_BUFFER
	assert(pSimpleBuffer);
	assert(pBuffer);
#endif

	// Init the base class members
	pBuffer->pPipe = (AT91PS_Pipe) 0;

	// Functions invoked by the pipe
	pBuffer->SetRdBuffer    = AT91F_SbSetRdBuffer;
	pBuffer->SetWrBuffer    = AT91F_SbSetWrBuffer;
	pBuffer->RstRdBuffer    = AT91F_SbRstRdBuffer;
	pBuffer->RstWrBuffer    = AT91F_SbRstWrBuffer;
	pBuffer->MsgWritten     = AT91F_SbMsgWritten;
	pBuffer->MsgRead        = AT91F_SbMsgRead;
	// Functions invoked by the peripheral
	pBuffer->GetWrBuffer    = AT91F_SbGetWrBuffer;
	pBuffer->GetRdBuffer    = AT91F_SbGetRdBuffer;
	pBuffer->EmptyWrBuffer  = AT91F_SbEmptyWrBuffer;
	pBuffer->FillRdBuffer   = AT91F_SbFillRdBuffer;
	pBuffer->pChild         = pSimpleBuffer;

	// Init class members
	pSimpleBuffer->szRdBuffer = 0;
	pSimpleBuffer->szWrBuffer = 0;
	pSimpleBuffer->pRdBuffer  = (char *) 0;
	pSimpleBuffer->pWrBuffer  = (char const *)0;
	return pBuffer;
}


//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbSetRdBuffer()
//* Object              : Prepare a new read buffer
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_BufferStatus AT91F_SbSetRdBuffer(
	AT91PS_Buffer  pBuffer,
	char           *pData,
	unsigned int   dataSize)
{
	AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pChild);

#ifdef DBG_DRV_BUFFER
	assert(pSBuffer);
#endif
	pSBuffer->pRdBuffer  = pData;
	pSBuffer->szRdBuffer = dataSize;
	return AT91C_BUFFER_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbSetWrBuffer()
//* Object              : Prepare a new write buffer
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_BufferStatus AT91F_SbSetWrBuffer(
	AT91PS_Buffer pBuffer,
	char const    *pData,
	unsigned int  dataSize)
{
	AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pChild);

#ifdef DBG_DRV_BUFFER
	assert(pSBuffer);
#endif
	pSBuffer->pWrBuffer  = pData;
	pSBuffer->szWrBuffer = dataSize;
	return AT91C_BUFFER_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbRstRdBuffer()
//* Object              : Empty the read buffer
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_BufferStatus AT91F_SbRstRdBuffer(
	AT91PS_Buffer pBuffer)
{
		AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pChild);

	#ifdef DBG_DRV_BUFFER
		assert(pSBuffer);
	#endif
		pSBuffer->pRdBuffer  = 0;
		pSBuffer->szRdBuffer = 0;
		return AT91C_BUFFER_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbRstWrBuffer()
//* Object              : Empty the write buffer
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_BufferStatus AT91F_SbRstWrBuffer(
	AT91PS_Buffer pBuffer)
{
		AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pChild);

	#ifdef DBG_DRV_BUFFER
		assert(pSBuffer);
	#endif
		pSBuffer->pWrBuffer = (char *) 0;
		pSBuffer->szWrBuffer = 0;
		return AT91C_BUFFER_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbMsgWritten()
//* Object              : Check that the message has been written
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
char AT91F_SbMsgWritten(
	AT91PS_Buffer pBuffer,
	char const *pMsg)
{
	AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pChild);

	#ifdef DBG_DRV_BUFFER
		assert(pSBuffer);
	#endif

	return ( !(pSBuffer->szWrBuffer > 0) );
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbMsgRead()
//* Object              : Check that the message has been read
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
char AT91F_SbMsgRead(
	AT91PS_Buffer pBuffer,
	char const *pMsg)
{
	AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pChild);

	#ifdef DBG_DRV_BUFFER
		assert(pSBuffer);
	#endif

	return ( !(pSBuffer->szRdBuffer > 0) );
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbGetWrBuffer()
//* Object              : Check that the message has been read
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_BufferStatus AT91F_SbGetWrBuffer(
	AT91PS_Buffer pBuffer,
	char const ** pData,
	unsigned int  *pSize)
{
	AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pChild);

	#ifdef DBG_DRV_BUFFER
		assert(pSBuffer);
	#endif
	*pData = pSBuffer->pWrBuffer;
	*pSize = pSBuffer->szWrBuffer;

	return AT91C_BUFFER_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbGetRdBuffer()
//* Object              : Check that the message has been read
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_BufferStatus AT91F_SbGetRdBuffer(
	AT91PS_Buffer pBuffer,
	char          **pData,
	unsigned int  *pSize)
{
	AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pChild);

	#ifdef DBG_DRV_BUFFER
		assert(pSBuffer);
	#endif
	*pData = pSBuffer->pRdBuffer;
	*pSize = pSBuffer->szRdBuffer;

	return AT91C_BUFFER_SUCCESS;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbEmptyWrBuffer()
//* Object              : A message has been written
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_BufferStatus AT91F_SbEmptyWrBuffer(
	AT91PS_Buffer pBuffer,
	unsigned int size)
{
	AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pChild);
	AT91PS_Pipe    pPipe;
	AT91S_BufferStatus status    = AT91C_BUFFER_SUCCESS;

	#ifdef DBG_DRV_BUFFER
		assert(pSBuffer);
	#endif
	// Update data pointer
	pSBuffer->pWrBuffer += size;
	// Update data counter
	if (pSBuffer->szWrBuffer < size) {
		pSBuffer->szWrBuffer = 0;
		status =  AT91C_BUFFER_UNDERRUN;
	}
	else {
		pSBuffer->szWrBuffer -= size;
	}

	// Invoke the callback
	if (pSBuffer->szWrBuffer == 0) {
		pPipe = pBuffer->pPipe;
		if (pPipe->WriteCallback)
			pPipe->WriteCallback(status, pPipe->pPrivateWriteData);
	}
	return status;
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbFillRdBuffer()
//* Object              : A message has been read
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
AT91S_BufferStatus AT91F_SbFillRdBuffer(
	AT91PS_Buffer pBuffer,
	unsigned int  size)
{
	AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pChild);
	AT91PS_Pipe    pPipe;
	AT91S_BufferStatus status = AT91C_BUFFER_SUCCESS;

	#ifdef DBG_DRV_BUFFER
		assert(pSBuffer);
	#endif
	// Update data pointer
	pSBuffer->pRdBuffer += size;
	// Update data counter
	if (pSBuffer->szRdBuffer < size) {
		pSBuffer->szRdBuffer = 0;
		status = AT91C_BUFFER_OVERFLOW;
	}
	else
		pSBuffer->szRdBuffer -= size;

	// Invoke the callback
	if (pSBuffer->szRdBuffer == 0) {
		pPipe = pBuffer->pPipe;
		if (pPipe->ReadCallback)
			pPipe->ReadCallback(status, pPipe->pPrivateReadData);
	}
	return status;
}

//*----------------------------------------------------------------------------
//* Function Name       : SbGetRdBuffer()
//* Object              : Check that the message has been read
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
#ifdef DBG_DRV_BUFFER
char const *AT91F_SbGetError(AT91S_BufferStatus errorNumber)
{
	switch(errorNumber & AT91C_BUFFER_ERROR) {
	case AT91C_BUFFER_SUCCESS:  return (SBufferErrMsg[0]);
	case AT91C_BUFFER_UNDERRUN: return (SBufferErrMsg[1]);
	case AT91C_BUFFER_OVERFLOW: return (SBufferErrMsg[2]);
	default:                    return (SBufferErrMsg[3]);
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbIsWrEmpty()
//* Object              : Check that the message has been written
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
char AT91F_SbIsWrEmpty(
	AT91PS_Buffer pBuffer)
{
	AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pPrivateData);

	#ifdef DBG_DRV_BUFFER
		assert(pSBuffer);
	#endif

	return ( !(pSBuffer->szWrBuffer > 0) );
}

//*----------------------------------------------------------------------------
//* Function Name       : AT91F_SbIsRdFull()
//* Object              : Check that the message has been read
//* Input Parameters    :
//* Output Parameters   :
//*----------------------------------------------------------------------------
char AT91F_SbIsRdFull(
	AT91PS_Buffer pBuffer)
{
	AT91PS_SBuffer pSBuffer = (AT91PS_SBuffer)(pBuffer->pPrivateData);

	#ifdef DBG_DRV_BUFFER
		assert(pSBuffer);
	#endif

	return ( !(pSBuffer->szRdBuffer > 0) );
}

#endif

