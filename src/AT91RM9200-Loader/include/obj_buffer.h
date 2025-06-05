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
#ifndef obj_buffer_h
#define obj_buffer_h

// Return values
#define AT91C_BUFFER_SUCCESS		   0
#define AT91C_BUFFER_ERROR_SHIFT      16
#define AT91C_BUFFER_ERROR            (0x0F << AT91C_BUFFER_ERROR_SHIFT)

#define AT91C_BUFFER_OVERFLOW         (0x01 << AT91C_BUFFER_ERROR_SHIFT)
#define AT91C_BUFFER_UNDERRUN         (0x02 << AT91C_BUFFER_ERROR_SHIFT)

typedef unsigned int AT91S_BufferStatus;

struct _AT91S_Pipe;

// This structure is a virtual object of a buffer
typedef struct _AT91S_Buffer
{
	struct _AT91S_Pipe *pPipe;
	void *pChild;

	// Functions invoked by the pipe
	AT91S_BufferStatus (*SetRdBuffer)     (struct _AT91S_Buffer *pSBuffer, char *pBuffer, unsigned int Size);
	AT91S_BufferStatus (*SetWrBuffer)     (struct _AT91S_Buffer *pSBuffer, char const *pBuffer, unsigned int Size);
	AT91S_BufferStatus (*RstRdBuffer)     (struct _AT91S_Buffer *pSBuffer);
	AT91S_BufferStatus (*RstWrBuffer)     (struct _AT91S_Buffer *pSBuffer);
	char (*MsgWritten)      (struct _AT91S_Buffer *pSBuffer, char const *pBuffer);
	char (*MsgRead)         (struct _AT91S_Buffer *pSBuffer, char const *pBuffer);
	// Functions invoked by the peripheral
	AT91S_BufferStatus (*GetWrBuffer)     (struct _AT91S_Buffer *pSBuffer, char const **pData, unsigned int *pSize);
	AT91S_BufferStatus (*GetRdBuffer)     (struct _AT91S_Buffer *pSBuffer, char **pData, unsigned int *pSize);
	AT91S_BufferStatus (*EmptyWrBuffer)   (struct _AT91S_Buffer *pSBuffer, unsigned int size);
	AT91S_BufferStatus (*FillRdBuffer)    (struct _AT91S_Buffer *pSBuffer, unsigned int size);
	char (*IsWrEmpty)      (struct _AT91S_Buffer *pSBuffer);
	char (*IsRdFull)       (struct _AT91S_Buffer *pSBuffer);
} AT91S_Buffer, *AT91PS_Buffer;

typedef AT91S_BufferStatus (*AT91PF_BufferSetRd)      (AT91PS_Buffer, char*, unsigned int);
typedef AT91S_BufferStatus (*AT91PF_BufferSetWr)      (AT91PS_Buffer, char const *, unsigned int);
typedef AT91S_BufferStatus (*AT91PF_BufferRstRd)      (AT91PS_Buffer);
typedef AT91S_BufferStatus (*AT91PF_BufferRstWr)      (AT91PS_Buffer);
typedef char (*AT91PF_BufferMsgWritten) (AT91PS_Buffer, char const *);
typedef char (*AT91PF_BufferMsgRead)    (AT91PS_Buffer, char const *);
typedef AT91S_BufferStatus (*AT91PF_BufferGetWr)      (AT91PS_Buffer, char *const*, unsigned int *);
typedef AT91S_BufferStatus (*AT91PF_BufferGetRd)      (AT91PS_Buffer, char **, unsigned int *);
typedef AT91S_BufferStatus (*AT91PF_BufferEmptyWr)    (AT91PS_Buffer, unsigned int);
typedef AT91S_BufferStatus (*AT91PF_BufferFillRd)     (AT91PS_Buffer, unsigned int);
typedef char (*AT91S_BufferIsWrEmpty)   (struct _AT91S_Buffer *pSBuffer);
typedef char (*AT91S_BufferIsRdFull)    (struct _AT91S_Buffer *pSBuffer);

#endif

