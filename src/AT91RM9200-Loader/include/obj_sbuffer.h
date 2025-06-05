//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : drv_buffer.h
//* Object              : Application pipe interface
//*
//* 1.0 20/06/01 	    : ODi Creation
//*----------------------------------------------------------------------------
#ifndef obj_sbuffer_h
#define obj_sbuffer_h

#include "services/obj_buffer.h"

// ===========================================================================================
// SimpleBuffer definition
//
// This structure is pointed by pRealBuffer field in the SBuffer
// It contains usefull information for a real implementation of
// a SBuffer object.
// The application just create an instance of SSBUffer and SBuffer,
// call OpenSimpleBuffer, and continue using SBuffer instance

typedef struct _AT91S_SBuffer
{
	AT91S_Buffer parent;
	char         *pRdBuffer;
	char const   *pWrBuffer;
	unsigned int szRdBuffer;
	unsigned int szWrBuffer;
	unsigned int stRdBuffer;
	unsigned int stWrBuffer;
} AT91S_SBuffer, *AT91PS_SBuffer;

typedef AT91PS_Buffer (*AT91PF_OpenSBuffer) (AT91PS_SBuffer);

// This function is called by the application
extern AT91PS_Buffer AT91F_OpenSBuffer(AT91PS_SBuffer pBuffer);

// Following functions must be called through AT91S_Buffer pointers

// Functions invoked by the pipe
extern AT91S_BufferStatus AT91F_SbSetRdBuffer  (AT91PS_Buffer pBuffer, char *pData, unsigned int Size);
extern AT91S_BufferStatus AT91F_SbSetWrBuffer  (AT91PS_Buffer pBuffer, char const *pData, unsigned int Size);
extern AT91S_BufferStatus AT91F_SbRstRdBuffer  (AT91PS_Buffer pBuffer);
extern AT91S_BufferStatus AT91F_SbRstWrBuffer  (AT91PS_Buffer pBuffer);
extern char AT91F_SbMsgWritten   (AT91PS_Buffer pBuffer, char const *pMsg);
extern char AT91F_SbMsgRead      (AT91PS_Buffer pBuffer, char const *pMsg);
// Functions invoked by the peripheral
extern AT91S_BufferStatus AT91F_SbGetWrBuffer  (AT91PS_Buffer pBuffer, char const **pData, unsigned int *pSize);
extern AT91S_BufferStatus AT91F_SbGetRdBuffer  (AT91PS_Buffer pBuffer, char **pData, unsigned int *pSize);
extern AT91S_BufferStatus AT91F_SbEmptyWrBuffer(AT91PS_Buffer pBuffer, unsigned int size);
extern AT91S_BufferStatus AT91F_SbFillRdBuffer (AT91PS_Buffer pBuffer, unsigned int size);
extern char AT91F_SbIsWrEmpty   (AT91PS_Buffer pBuffer);
extern char AT91F_SbIsRdFull    (AT91PS_Buffer pBuffer);

#ifdef DBG_DRV_BUFFER
extern char const *AT91F_SbGetError(AT91S_BufferStatus errorNumber);
#endif

// ===========================================================================================

#endif
