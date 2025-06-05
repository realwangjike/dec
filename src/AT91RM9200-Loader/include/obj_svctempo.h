//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : drv_timer.h
//* Object              :
//*
//* 1.0 20/06/01 	    :
//*----------------------------------------------------------------------------
#ifndef obj_svctempo_h
#define obj_svctempo_h

#include "periph/system_timer/lib_st.h"

//* ====================================================================================
#define AT91C_OPEN_CTRLTEMPO_SUCCESS	0
#define AT91C_ERROR_OPEN_CTRLTEMPO		1
#define AT91C_START_OK					2
#define AT91C_STOP_OK					3
#define AT91C_TIMEOUT_REACHED			4

typedef enum _AT91E_SvcTempo {
	AT91E_SVCTEMPO_DIS,
	AT91E_SVCTEMPO_EN
} AT91E_SvcTempo;

//* ====================================================================================
typedef unsigned int AT91S_TempoStatus;

// AT91S_SvcTempo
typedef struct _AT91S_SvcTempo
{

	// Methods:
	AT91S_TempoStatus (*Start)  (
		struct _AT91S_SvcTempo *pSvc,
		unsigned int timeout,
		unsigned int reload,
		void (*callback) (AT91S_TempoStatus, void *),
		void *pData);
	AT91S_TempoStatus (*Stop)   (struct _AT91S_SvcTempo *pSvc);

	struct _AT91S_SvcTempo *pPreviousTempo;
	struct _AT91S_SvcTempo *pNextTempo;

	// Data
	unsigned int TickTempo;	//* timeout value
	unsigned int ReloadTempo;//* Reload value for periodic execution
	void (*TempoCallback)(AT91S_TempoStatus, void *);
	void *pPrivateData;
	AT91E_SvcTempo flag;
} AT91S_SvcTempo, *AT91PS_SvcTempo;


// AT91S_CtrlTempo
typedef struct _AT91S_CtlTempo
{
	// Members:

	// Start and stop for Timer	hardware
	AT91S_TempoStatus (*CtlTempoStart)  (void *pTimer);
	AT91S_TempoStatus (*CtlTempoStop)   (void *pTimer);

	// Start and stop for Tempo service
	AT91S_TempoStatus (*SvcTempoStart)  (
		struct _AT91S_SvcTempo *pSvc,
		unsigned int timeout,
		unsigned int reload,
		void (*callback) (AT91S_TempoStatus, void *),
		void *pData);
	AT91S_TempoStatus (*SvcTempoStop)   (struct _AT91S_SvcTempo *pSvc);
	AT91S_TempoStatus (*CtlTempoSetTime)(struct _AT91S_CtlTempo *pCtrl, unsigned int NewTime);
	AT91S_TempoStatus (*CtlTempoGetTime)(struct _AT91S_CtlTempo *pCtrl);
	AT91S_TempoStatus (*CtlTempoIsStart)(struct _AT91S_CtlTempo *pCtrl);
	AT91S_TempoStatus (*CtlTempoCreate) (
								struct _AT91S_CtlTempo *pCtrl,
								struct _AT91S_SvcTempo *pTempo);
	AT91S_TempoStatus (*CtlTempoRemove) (
								struct _AT91S_CtlTempo *pCtrl,
								struct _AT91S_SvcTempo *pTempo);
	AT91S_TempoStatus (*CtlTempoTick)   (struct _AT91S_CtlTempo *pCtrl);

	// Data:

	void *pPrivateData;     // Pointer to devived class
	void const *pTimer;			// hardware
	AT91PS_SvcTempo pFirstTempo;
	AT91PS_SvcTempo pNewTempo;
} AT91S_CtlTempo, *AT91PS_CtlTempo;



// Types used in AT91S_CtrlTempo
typedef AT91S_TempoStatus (*AT91PF_OpenCtlTempo)   ( AT91PS_CtlTempo, void const *);
typedef AT91S_TempoStatus (*AT91PF_CtlTempoStart)  (void *);
typedef AT91S_TempoStatus (*AT91PF_CtlTempoStop)   (void *);
typedef AT91S_TempoStatus (*AT91PF_CtlTempoSetTime)(AT91PS_CtlTempo, unsigned int);
typedef AT91S_TempoStatus (*AT91PF_CtlTempoGetTime)(AT91PS_CtlTempo);
typedef AT91S_TempoStatus (*AT91PF_CtlTempoIsStart)(AT91PS_CtlTempo);
typedef	AT91S_TempoStatus (*AT91PF_CtlTempoCreate) (AT91PS_CtlTempo, AT91PS_SvcTempo);
typedef	AT91S_TempoStatus (*AT91PF_CtlTempoRemove) (AT91PS_CtlTempo, AT91PS_SvcTempo);
typedef	AT91S_TempoStatus (*AT91PF_CtlTempoTick)   (AT91PS_CtlTempo);

// Types used in AT91S_SvcTempo
typedef void (*AT91PF_SvcTempoCallback) (AT91S_TempoStatus TempoStatus, void *);
typedef AT91S_TempoStatus (*AT91PF_SvcTempoStart)  (AT91PS_SvcTempo pSvc, unsigned int, unsigned int, AT91PF_SvcTempoCallback, void *);
typedef AT91S_TempoStatus (*AT91PF_SvcTempoStop)   (AT91PS_SvcTempo pSvc);



//* ====================================================================================

// This function is called by the application.
extern AT91S_TempoStatus AT91F_OpenCtlTempo( AT91PS_CtlTempo pCtrlTempo, void const *pTempoTimer );

extern AT91S_TempoStatus AT91F_STStart   (void *);
extern AT91S_TempoStatus AT91F_STStop    (void *);
extern AT91S_TempoStatus AT91F_STSetTime (AT91PS_CtlTempo, unsigned int);
extern AT91S_TempoStatus AT91F_STGetTime (AT91PS_CtlTempo);
extern AT91S_TempoStatus AT91F_STIsStart (AT91PS_CtlTempo);
extern AT91S_TempoStatus AT91F_CtlTempoCreate (AT91PS_CtlTempo, AT91PS_SvcTempo);
extern AT91S_TempoStatus AT91F_CtlTempoRemove (AT91PS_CtlTempo, AT91PS_SvcTempo);
extern AT91S_TempoStatus AT91F_CtlTempoTick   (AT91PS_CtlTempo);
extern AT91S_TempoStatus AT91F_SvcTempoStart (
		AT91PS_SvcTempo pSvc,
		unsigned int timeout,
		unsigned int reload,
		void (*callback) (AT91S_TempoStatus, void *),
		void *pData);
extern AT91S_TempoStatus AT91F_SvcTempoStop (AT91PS_SvcTempo);



#endif // obj_svctempo_h
