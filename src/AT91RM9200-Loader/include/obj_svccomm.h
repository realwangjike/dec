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
#ifndef obj_svccomm_h
#define obj_svccomm_h


// Following types are defined in another header files
struct _AT91S_Buffer;

// Constants:
#define AT91C_COMMSVC_SUCCESS     0
#define AT91C_COMMSVC_ERROR_SHIFT 8
#define AT91C_COMMSVC_ERROR       (0x0f << AT91C_COMMSVC_ERROR_SHIFT)

typedef unsigned int AT91S_SvcCommStatus;


// AT91S_Service definition
// This structure is an abstraction of a communication peripheral

typedef struct _AT91S_Service
{
	// Methods:
	AT91S_SvcCommStatus (*Reset)  (struct _AT91S_Service *pService);
	AT91S_SvcCommStatus (*StartTx)(struct _AT91S_Service *pService);
	AT91S_SvcCommStatus (*StartRx)(struct _AT91S_Service *pService);
	AT91S_SvcCommStatus (*StopTx) (struct _AT91S_Service *pService);
	AT91S_SvcCommStatus (*StopRx) (struct _AT91S_Service *pService);
	char                (*TxReady)(struct _AT91S_Service *pService);
	char                (*RxReady)(struct _AT91S_Service *pService);
	// Data:
	struct _AT91S_Buffer *pBuffer; // Link to a buffer object
	void *pChild;
} AT91S_SvcComm, *AT91PS_SvcComm;

// Types used in AT91S_Service:
typedef AT91S_SvcCommStatus (*AT91PF_SvcCommReset)   (AT91PS_SvcComm);
typedef AT91S_SvcCommStatus (*AT91PF_SvcCommStartTx) (AT91PS_SvcComm);
typedef AT91S_SvcCommStatus (*AT91PF_SvcCommStartRx) (AT91PS_SvcComm);
typedef AT91S_SvcCommStatus (*AT91PF_SvcCommStopTx)  (AT91PS_SvcComm);
typedef AT91S_SvcCommStatus (*AT91PF_SvcCommStopRx)  (AT91PS_SvcComm);
typedef char                (*AT91PF_SvcCommTxReady) (AT91PS_SvcComm);
typedef char                (*AT91PF_SvcCommRxReady) (AT91PS_SvcComm);

#endif // obj_svccomm_h
