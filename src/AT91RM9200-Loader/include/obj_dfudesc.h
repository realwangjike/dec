//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : obj_dfudesc.h
//* Object              : DFU filter.
//*
//* 1.0 07/24/01 ODi    : Creation
//*----------------------------------------------------------------------------
#ifndef obj_dfudesc_h
#define obj_dfudesc_h

#include "services/obj_usbdesc.h"
#include "services/obj_svctempo.h"
#include "services/obj_svcudp.h"

/* ====================================================================== */
/* DFU definitions extracted from the USB Device Firmware Upgrade         */
/* Specification, revision 1.0                                            */
/* ====================================================================== */

/* Run-Time DFU Functional Descriptor */
/* This descriptor is identical for both run-time and the DFU mode */
/* descriptor set */

typedef __packed struct _AT91S_DfuFDesc {
	volatile char  bLength;
	volatile char  bDescriptorType;
	volatile char  bmAttributes;
	volatile unsigned short wDetachTimeout;
	volatile unsigned short wTransferSize;
} AT91S_DfuFDesc, *AT91PS_DfuFDesc;

/* Device status structure as defined in */
/* USB Device Firmware Upgrade Specification, rev 1.1 */
typedef __packed struct _AT91S_DfuStatus {
	volatile char bStatus;
	volatile char bwPollTimeout[3];
	volatile char bState;
	char iString;
} AT91S_DfuStatus, *AT91PS_DfuStatus;

__inline char bmAttributes(AT91S_DfuFDesc const *pUSBDfuFDesc)
{
	char const *pChar = (char *) pUSBDfuFDesc;
	return pChar[2];
}

__inline unsigned short wDetachTimeOut(AT91S_DfuFDesc const *pUSBDfuFDesc)
{
	char const *pChar = (char *) pUSBDfuFDesc;
	return (((pChar[4]) << 8) | pChar[3]);
}

__inline unsigned short wTransferSize(AT91S_DfuFDesc const *pUSBDfuFDesc)
{
	char const *pChar = (char *) pUSBDfuFDesc;
	return ((pChar[6] << 8) | pChar[5]);
}

__inline unsigned int bwPollTimeout(AT91S_DfuStatus const *pStatus)
{
	char const *pChar = (char *) pStatus;
	return ((pChar[3] << 16) | (pChar[2] << 8) | pChar[1]);
}

/* DFU class-Specific Request Values */
#define AT91C_DFU_DETACH    0
#define AT91C_DFU_DNLOAD    1
#define AT91C_DFU_UPLOAD    2
#define AT91C_DFU_GETSTATUS 3
#define AT91C_DFU_CLRSTATUS 4
#define AT91C_DFU_GETSTATE  5
#define AT91C_DFU_ABORT     6

/* DFU state values, as defined in */
/* USB Device Firmware Upgrade Specification, rev 1.1 */
#define AT91C_DFU_appIDLE                0
#define AT91C_DFU_appDETACH              1
#define AT91C_DFU_dfuIDLE                2
#define AT91C_DFU_dfuDNLOAD_SYNC         3
#define AT91C_DFU_dfuDNBUSY              4
#define AT91C_DFU_dfuDNLOAD_IDLE         5
#define AT91C_DFU_dfuMANIFEST_SYNC       6
#define AT91C_DFU_dfuMANIFEST            7
#define AT91C_DFU_dfuMANIFEST_WAIT_RESET 8
#define AT91C_DFU_dfuUPLOAD_IDLE         9
#define AT91C_DFU_dfuERROR               10

/* DFU status values, as defined in */
/* USB Device Firmware Upgrade Specification, rev 1.1 */
#define AT91C_DFU_OK              0x00
#define AT91C_DFU_errTARGET       0x01
#define AT91C_DFU_errFILE         0x02
#define AT91C_DFU_errWRITE        0x03
#define AT91C_DFU_errERASE        0x04
#define AT91C_DFU_errCHECK_ERASED 0x05
#define AT91C_DFU_errPROG         0x06
#define AT91C_DFU_errVERIFY       0x07
#define AT91C_DFU_errADDRESS      0x08
#define AT91C_DFU_errNOTDONE      0x09
#define AT91C_DFU_errFIRMWARE     0x0A
#define AT91C_DFU_errVENDOR       0x0B
#define AT91C_DFU_errUSBR         0x0C
#define AT91C_DFU_errPOR          0x0D
#define AT91C_DFU_errUNKNOWN      0x0E
#define AT91C_DFU_errSTALLPKT     0x0F

/* ====================================================================== */
/* DFU definitions used in lib_dfu.c                                      */
/* ====================================================================== */


typedef struct _AT91S_DfuDesc {
	AT91PS_Pipe  pPipe;     // Link to a AT91S_Pipe object
	AT91S_SvcTempo tempo; // Link to a AT91S_Tempo object

	// Private methods:
	void (*State0)  (struct _AT91S_DfuDesc *, AT91PS_UsbSetup);
	void (*State1)  (struct _AT91S_DfuDesc *, AT91PS_UsbSetup);
	void (*State2)  (struct _AT91S_DfuDesc *, AT91PS_UsbSetup);
	void (*State3)  (struct _AT91S_DfuDesc *, AT91PS_UsbSetup);
	void (*State4)  (struct _AT91S_DfuDesc *, AT91PS_UsbSetup);
	void (*State5)  (struct _AT91S_DfuDesc *, AT91PS_UsbSetup);
	void (*State6)  (struct _AT91S_DfuDesc *, AT91PS_UsbSetup);
	void (*State7)  (struct _AT91S_DfuDesc *, AT91PS_UsbSetup);
	void (*State8)  (struct _AT91S_DfuDesc *, AT91PS_UsbSetup);
	void (*State9)  (struct _AT91S_DfuDesc *, AT91PS_UsbSetup);
	void (*State10) (struct _AT91S_DfuDesc *, AT91PS_UsbSetup);

	void (*GetStatus) (struct _AT91S_DfuDesc *);
	void (*GetState)  (struct _AT91S_DfuDesc *);

	// public methods:
	void (*DispatchClassRequest) (AT91PS_UsbSetup, void *);
	void (*Download)(struct _AT91S_DfuDesc *, unsigned short, AT91PF_PipeReadCallBack);
	void (*Upload)(struct _AT91S_DfuDesc *, unsigned short);

	// Public data:
	AT91S_DfuStatus dfuStatus;
	AT91S_DfuFDesc const *pDfuFDesc;
	char manifestComplete;
	char blockComplete;

} AT91S_DfuDesc, *AT91PS_DfuDesc;

// Types definition:
typedef void (*AT91PF_DfuDescDownload)  (AT91PS_DfuDesc, unsigned short, AT91PF_PipeReadCallBack);
typedef void (*AT91PF_DfuDescUpload)  (AT91PS_DfuDesc, unsigned short);

typedef AT91PS_DfuDesc (*AT91PF_OpenDfuDesc) (
	AT91PS_DfuDesc, 
	AT91PS_Pipe, 
	AT91PS_CtlTempo, 
	char, 
	AT91S_DfuFDesc const *, 
	AT91PF_DfuDescDownload, 
	AT91PF_DfuDescUpload);
typedef void (*AT91PF_DfuDescDispatchClassRequest) (AT91PS_DfuDesc, void *);
typedef void (*AT91PF_DfuDescState0)  (AT91PS_DfuDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DfuDescState1)  (AT91PS_DfuDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DfuDescState2)  (AT91PS_DfuDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DfuDescState3)  (AT91PS_DfuDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DfuDescState4)  (AT91PS_DfuDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DfuDescState5)  (AT91PS_DfuDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DfuDescState6)  (AT91PS_DfuDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DfuDescState7)  (AT91PS_DfuDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DfuDescState8)  (AT91PS_DfuDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DfuDescState9)  (AT91PS_DfuDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DfuDescState10) (AT91PS_DfuDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DfuDescGetStatus) (AT91PS_DfuDesc);
typedef void (*AT91PF_DfuDescGetState)  (AT91PS_DfuDesc);

// Function declaration:
extern AT91PS_DfuDesc AT91F_OpenDfuDesc(AT91PS_DfuDesc, AT91PS_Pipe, AT91PS_CtlTempo, char, AT91S_DfuFDesc const *, AT91PF_DfuDescDownload, AT91PF_DfuDescUpload);

extern void AT91F_DfuDescDispatchClassRequest (AT91PS_UsbSetup, void *);
extern void AT91F_DfuDescState0  (AT91PS_DfuDesc, AT91PS_UsbSetup);
extern void AT91F_DfuDescState1  (AT91PS_DfuDesc, AT91PS_UsbSetup);
extern void AT91F_DfuDescState2  (AT91PS_DfuDesc, AT91PS_UsbSetup);
extern void AT91F_DfuDescState3  (AT91PS_DfuDesc, AT91PS_UsbSetup);
extern void AT91F_DfuDescState4  (AT91PS_DfuDesc, AT91PS_UsbSetup);
extern void AT91F_DfuDescState5  (AT91PS_DfuDesc, AT91PS_UsbSetup);
extern void AT91F_DfuDescState6  (AT91PS_DfuDesc, AT91PS_UsbSetup);
extern void AT91F_DfuDescState7  (AT91PS_DfuDesc, AT91PS_UsbSetup);
extern void AT91F_DfuDescState8  (AT91PS_DfuDesc, AT91PS_UsbSetup);
extern void AT91F_DfuDescState9  (AT91PS_DfuDesc, AT91PS_UsbSetup);
extern void AT91F_DfuDescState10 (AT91PS_DfuDesc, AT91PS_UsbSetup);
extern void AT91F_DfuDescGetStatus(AT91PS_DfuDesc);
extern void AT91F_DfuDescGetState (AT91PS_DfuDesc);
extern void AT91F_DfuDescSuccess  (AT91PS_DfuDesc);
extern void AT91F_DfuDescError    (AT91PS_DfuDesc);
extern void AT91F_DfuDescDetachTimeout(AT91S_TempoStatus status, void *pVoid);
extern void AT91F_DfuDescStatusPollTimeOut(AT91S_TempoStatus status, void *pVoid);
extern void AT91F_DfuDescDownload(AT91S_PipeStatus status, void *pVoid);
#endif // obj_dfudesc_h
