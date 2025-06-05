//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : drv_pipe.h
//* Object              : Application pipe interface
//*
//* 1.0 20/06/01 	    : ODi Creation
//*----------------------------------------------------------------------------
#ifndef obj_pipe_h
#define obj_pipe_h

//----------------------------------------------------------------------------
// To use a pipe, create a SPipe instance, and initialise it calling
// AT91F_PipeOpen
//----------------------------------------------------------------------------

// Following types are defined in external includes
#include "services/obj_buffer.h"
#include "services/obj_svccomm.h"

// Constants:
#define AT91C_PIPE_SUCCESS	      0
#define AT91C_PIPE_ERROR_SHIFT    8
#define AT91C_PIPE_ERROR          (0x0F << AT91C_PIPE_ERROR_SHIFT)

#define AT91C_PIPE_OPEN_FAILED    (1 << AT91C_PIPE_ERROR_SHIFT)
#define AT91C_PIPE_WRITE_FAILED   (2 << AT91C_PIPE_ERROR_SHIFT)
#define AT91C_PIPE_WRITE_ABORTED  (3 << AT91C_PIPE_ERROR_SHIFT)
#define AT91C_PIPE_READ_FAILED    (4 << AT91C_PIPE_ERROR_SHIFT)
#define AT91C_PIPE_READ_ABORTED   (5 << AT91C_PIPE_ERROR_SHIFT)
#define AT91C_PIPE_ABORT_FAILED   (6 << AT91C_PIPE_ERROR_SHIFT)
#define AT91C_PIPE_RESET_FAILED   (7 << AT91C_PIPE_ERROR_SHIFT)

/* _AT91S_Pipe stucture */
typedef unsigned int AT91S_PipeStatus;

typedef struct _AT91S_Pipe
{
	// A pipe is linked with a peripheral and a buffer
	AT91PS_SvcComm pSvcComm;
	AT91PS_Buffer  pBuffer;

	// Callback functions with their arguments
	void (*WriteCallback) (AT91S_PipeStatus, void *);
	void (*ReadCallback)  (AT91S_PipeStatus, void *);
	void *pPrivateReadData;
	void *pPrivateWriteData;

	// Pipe methods
	AT91S_PipeStatus (*Write) (
		struct _AT91S_Pipe   *pPipe,
		char const *         pData,
		unsigned int         size,
		void                 (*callback) (AT91S_PipeStatus, void *),
		void                 *privateData);
	AT91S_PipeStatus (*Read) (
		struct _AT91S_Pipe  *pPipe,
		char                *pData,
		unsigned int        size,
		void                (*callback) (AT91S_PipeStatus, void *),
		void                *privateData);
	AT91S_PipeStatus (*AbortWrite) (
		struct _AT91S_Pipe  *pPipe);
	AT91S_PipeStatus (*AbortRead) (
		struct _AT91S_Pipe *pPipe);
	AT91S_PipeStatus (*Reset) (
		struct _AT91S_Pipe *pPipe);
	char (*IsWritten) (
		struct _AT91S_Pipe *pPipe,
		char const *pVoid);
	char (*IsReceived) (
		struct _AT91S_Pipe *pPipe,
		char const *pVoid);
} AT91S_Pipe, *AT91PS_Pipe;

// types used in AT91S_Pipe
typedef AT91PS_Pipe (*AT91PF_OpenPipe)   (AT91PS_Pipe, AT91PS_SvcComm, AT91PS_Buffer);
typedef void (*AT91PF_PipeWriteCallBack) (AT91S_PipeStatus, void *);
typedef void (*AT91PF_PipeReadCallBack)  (AT91S_PipeStatus, void *);
typedef AT91S_PipeStatus (*AT91PF_PipeWrite) (AT91PS_Pipe, char const *, unsigned int, void (*) (AT91S_PipeStatus, void *),	void *);
typedef AT91S_PipeStatus (*AT91PF_PipeRead)  (AT91PS_Pipe, char const *, unsigned int, void (*) (AT91S_PipeStatus, void *),	void *);
typedef AT91S_PipeStatus (*AT91PF_PipeAbortWrite) (AT91PS_Pipe);
typedef AT91S_PipeStatus (*AT91PF_PipeAbortRead)  (AT91PS_Pipe);
typedef AT91S_PipeStatus (*AT91PF_PipeReset)      (AT91PS_Pipe);
typedef char (*AT91PF_PipeIsWritten)              (AT91PS_Pipe, char const *);
typedef char (*AT91PF_PipeIsReceived)             (AT91PS_Pipe, char const *);



// This function is called by the application
extern AT91PS_Pipe AT91F_OpenPipe(
	AT91PS_Pipe    pPipe,
	AT91PS_SvcComm pSvcComm,
	AT91PS_Buffer  pBuffer);

// Following functions are called through AT91S_Pipe pointers

extern AT91S_PipeStatus AT91F_PipeWrite(
	AT91PS_Pipe pPipe,
	char const *pVoid,
	unsigned int size,
	AT91PF_PipeWriteCallBack callback,
	void *privateData);
extern AT91S_PipeStatus AT91F_PipeRead(
	AT91PS_Pipe pPipe,
	char *pVoid,
	unsigned int Size,
	AT91PF_PipeReadCallBack callback,
	void *privateData);
extern AT91S_PipeStatus AT91F_PipeAbortWrite(AT91PS_Pipe pPipe);
extern AT91S_PipeStatus AT91F_PipeAbortRead(AT91PS_Pipe pPipe);
extern AT91S_PipeStatus AT91F_PipeReset(AT91PS_Pipe pPipe);
extern char AT91F_PipeMsgWritten(AT91PS_Pipe pPipe, char const *pVoid);
extern char AT91F_PipeMsgReceived(AT91PS_Pipe pPipe, char const *pVoid);

#ifdef DBG_DRV_PIPE
// This function parse the error number and return a string
// describing the error message
extern char const *AT91F_PipeGetError(AT91S_PipeStatus msgId);
#endif

#endif // obj_pipe_h
