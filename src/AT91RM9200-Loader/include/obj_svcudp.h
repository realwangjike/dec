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
#ifndef obj_svcudp_h
#define obj_svcudp_h

#include "periph/udp/lib_udp.h"
#include "services/obj_pipe.h"

// Constants:
#define AT91C_SVCUDP_RD_FAILED         (0x01 << AT91C_COMMSVC_ERROR_SHIFT)
#define AT91C_SVCUDP_RD_BUFFER_NOT_RDY (0x02 << AT91C_COMMSVC_ERROR_SHIFT)
#define AT91C_SVCUDP_RD_NOT_RDY        (0x03 << AT91C_COMMSVC_ERROR_SHIFT)
#define AT91C_SVCUDP_WR_FAILED         (0x04 << AT91C_COMMSVC_ERROR_SHIFT)
#define AT91C_SVCUDP_WR_NOT_RDY        (0x05 << AT91C_COMMSVC_ERROR_SHIFT)
#define AT91C_SVCUDP_WR_ABORTED        (0x06 << AT91C_COMMSVC_ERROR_SHIFT)

typedef enum _AT91E_UdpSvc {
	AT91C_SVCUDP_CTL_RD,
	AT91C_SVCUDP_CTL_WR
} AT91E_UdpSvc;

//* ====================================================================================
//* Following structure is used by SPipe to refer to the USB device peripheral endpoint
typedef struct _AT91S_SvcUdp {
	AT91S_SvcCommStatus (*HandleRx) (struct _AT91S_SvcUdp *);
	AT91S_SvcCommStatus (*HandleTx) (struct _AT91S_SvcUdp *);
	AT91S_SvcComm  parent;      // Base class
	AT91PS_UDP     pUdp;         // Pointer to the USB registers
	char           epId;         // USB device endpoint number
	unsigned int   epSize;       // USB device endpoint size disussed during the enumeration
	unsigned char  bank;
	AT91E_UdpSvc   operation;

} AT91S_SvcUdp, *AT91PS_SvcUdp;

typedef AT91PS_SvcComm (*AT91PF_OpenSvcUdp) ( AT91PS_SvcUdp, AT91PS_UDP, char, unsigned int);
typedef AT91S_SvcCommStatus (*AT91PF_SvcUdpProtocol) (AT91PS_SvcUdp, unsigned int);
//* ====================================================================================

// This function is called by the application.
extern AT91PS_SvcComm AT91F_OpenSvcUdp(
	AT91PS_SvcUdp pEndpoint,
	AT91PS_UDP    pUdp,
	char          epNumber,
	unsigned int  epSize);

// These functions are called through AT91PS_Service pointers
//extern AT91S_SvcCommStatus AT91F_SvcUdpProtocol(AT91PS_SvcUdp pSvcUdp);
extern AT91S_SvcCommStatus AT91F_SvcUdpControlHandleRx(AT91PS_SvcUdp pSvcUdp);
extern AT91S_SvcCommStatus AT91F_SvcUdpControlHandleTx(AT91PS_SvcUdp pSvcUdp);

extern AT91S_SvcCommStatus AT91F_SvcUdpStartTx(AT91PS_SvcComm pService);
extern AT91S_SvcCommStatus AT91F_SvcUdpStartRx(AT91PS_SvcComm pService);
extern AT91S_SvcCommStatus AT91F_SvcUdpStopTx(AT91PS_SvcComm pService);
extern AT91S_SvcCommStatus AT91F_SvcUdpStopRx(AT91PS_SvcComm pService);
extern AT91S_SvcCommStatus AT91F_SvcUdpSend(AT91PS_SvcComm pService);
extern AT91S_SvcCommStatus AT91F_SvcUdpReceive(AT91PS_SvcComm pService);
extern char                AT91F_SvcUdpTxReady(AT91PS_SvcComm pService);
extern char                AT91F_SvcUdpRxReady(AT91PS_SvcComm pService);

#ifdef DBG_DRV_PERIPH
extern char const *AT91F_SvcUdpGetError(AT91S_ServiceStatus errorNumber)
#endif // DBG_DRV_PERIPH

#endif // obj_svcudp_h
