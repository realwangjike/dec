//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_dfu.c
//* Object              : USB peripheral validation.
//*
//* 1.0 07/24/01 ODi    : Creation
//*----------------------------------------------------------------------------
#include "services/obj_dfudesc.h"

/* ============================================================================ */
/* This function is called */
/* ============================================================================ */

/* ***************************************************** */
/* AT91F_OpenDfuDesc                                     */
/*                                                       */
/* Create a  AT91S_DfuDesc object and initializes it     */
/* ***************************************************** */
AT91PS_DfuDesc AT91F_OpenDfuDesc(
	AT91PS_DfuDesc pDfu,
	AT91PS_Pipe pPipe,
	AT91PS_CtlTempo pCtlTempo,
	char bState,
	AT91S_DfuFDesc const  *pDfuFDesc,
	AT91PF_DfuDescDownload Download,
	AT91PF_DfuDescUpload Upload)
{
	pDfu->pPipe = pPipe;
	pDfu->dfuStatus.bState = bState;
	pDfu->dfuStatus.bStatus = AT91C_DFU_OK;
	pDfu->dfuStatus.bwPollTimeout[0] = 0x00;
	pDfu->dfuStatus.bwPollTimeout[1] = 0x01;
	pDfu->dfuStatus.bwPollTimeout[2] = 0x00;
 

	pDfu->pDfuFDesc = pDfuFDesc;

	pDfu->manifestComplete = 0;
	pDfu->blockComplete    = 0;
	pDfu->DispatchClassRequest = AT91F_DfuDescDispatchClassRequest;

	pDfu->State0  = AT91F_DfuDescState0;
	pDfu->State1  = AT91F_DfuDescState1;
	pDfu->State2  = AT91F_DfuDescState2;
	pDfu->State3  = AT91F_DfuDescState3;
	pDfu->State4  = AT91F_DfuDescState4;
	pDfu->State5  = AT91F_DfuDescState5;
	pDfu->State6  = AT91F_DfuDescState6;
	pDfu->State7  = AT91F_DfuDescState7;
	pDfu->State8  = AT91F_DfuDescState8;
	pDfu->State9  = AT91F_DfuDescState9;
	pDfu->State10 = AT91F_DfuDescState10;
	pDfu->GetState  = AT91F_DfuDescGetState;
	pDfu->GetStatus = AT91F_DfuDescGetStatus;
	pDfu->Download = Download;
	pDfu->Upload   = Upload;

	// Attach the tempo to a tempo controler
	pCtlTempo->CtlTempoCreate(pCtlTempo, &(pDfu->tempo));
	return pDfu;
}

/* ***************************************************** */
/* AT91F_DfuDescDispatchRequest                                 */
/*                                                       */
/* Handle DFU class specific requests. According to the  */
/* current DFU state, it calls a static functions which  */
/* will call class specific requests.                    */
/* ***************************************************** */
void AT91F_DfuDescDispatchClassRequest (
	AT91PS_UsbSetup pSetup,
	void    *pVoid)        /* DFU handler */
{
	AT91PS_DfuDesc pDfuDesc = (AT91PS_DfuDesc) pVoid;
	switch (pDfuDesc->dfuStatus.bState) {
	case AT91C_DFU_appIDLE:                pDfuDesc->State0 (pDfuDesc, pSetup); break;
	case AT91C_DFU_appDETACH:              pDfuDesc->State1 (pDfuDesc, pSetup); break;
	case AT91C_DFU_dfuIDLE:                pDfuDesc->State2 (pDfuDesc, pSetup); break;
	case AT91C_DFU_dfuDNLOAD_SYNC:         pDfuDesc->State3 (pDfuDesc, pSetup); break;
	case AT91C_DFU_dfuDNBUSY:              pDfuDesc->State4 (pDfuDesc, pSetup); break;
	case AT91C_DFU_dfuDNLOAD_IDLE:         pDfuDesc->State5 (pDfuDesc, pSetup); break;
	case AT91C_DFU_dfuMANIFEST_SYNC:       pDfuDesc->State6 (pDfuDesc, pSetup); break;
	case AT91C_DFU_dfuMANIFEST:            pDfuDesc->State7 (pDfuDesc, pSetup); break;
	case AT91C_DFU_dfuMANIFEST_WAIT_RESET: pDfuDesc->State8 (pDfuDesc, pSetup); break;
	case AT91C_DFU_dfuUPLOAD_IDLE:         pDfuDesc->State9 (pDfuDesc, pSetup); break;
	default:                               pDfuDesc->State10(pDfuDesc, pSetup); break;
	}
}

/* ============================================================================ */
/* Following functions are called through usbDfuDispatchRequest()->dfuStatexx() */
/* ============================================================================ */

/* ***************************************************** */
/* pDfu->GetStatus                                       */
/*                                                       */
/* ***************************************************** */
void AT91F_DfuDescGetStatus(AT91PS_DfuDesc pDfu)
{
	AT91PS_Pipe pPipe = pDfu->pPipe;

	pPipe->Write(pPipe, (char *) &(pDfu->dfuStatus), 6, (AT91PF_PipeWriteCallBack) 0, (void *) 0);
}

/* ***************************************************** */
/* pDfu->GetState                                        */
/*                                                       */
/* ***************************************************** */
void AT91F_DfuDescGetState(AT91PS_DfuDesc pDfu)
{
	AT91PS_Pipe pPipe = pDfu->pPipe;

	pPipe->Write(pPipe, (char *) &(pDfu->dfuStatus.bState), 1, (AT91PF_PipeWriteCallBack) 0, (void *) 0);
}

/* ***************************************************** */
/* AT91F_DfuDescDownload: called at the end of download  */
/*                                                       */
/* ***************************************************** */
void AT91F_DfuDescDownload(AT91S_PipeStatus status, void *pVoid)
{
	AT91PS_DfuDesc pDfu = (AT91PS_DfuDesc) pVoid;

	// Notify the end of download
	pDfu->blockComplete = 1;

	// Set the status
	pDfu->dfuStatus.bStatus = status;
	
	// Acknowledge
	AT91F_DfuDescSuccess(pDfu);

}

/* ***************************************************** */
/* pDfu->AT91F_DfuDescDfuSuccess                         */
/*                                                       */
/* ***************************************************** */
void AT91F_DfuDescSuccess(AT91PS_DfuDesc pDfu)
{
	AT91PS_Pipe pPipe = pDfu->pPipe;

	pPipe->Write(pPipe, (char *) 0, 0, (AT91PF_PipeWriteCallBack) 0, (void *) 0);
}

/* ***************************************************** */
/* pDfu->DfuError                                        */
/*                                                       */
/* ***************************************************** */
void AT91F_DfuDescError(AT91PS_DfuDesc pDfu)
{
	AT91PS_Pipe pPipe = pDfu->pPipe;

	pPipe->AbortWrite(pPipe);
}

/* ============================================================================ */
/* Following functions are supposed to be called at the end of a timeout        */
/* Initialisation of the timeout is done using pTimer->initTimeout(...)         */
/* ============================================================================ */
void AT91F_DfuDescDetachTimeout(AT91S_TempoStatus status, void *pVoid)
{
	AT91PS_DfuDesc pDfu = (AT91PS_DfuDesc) pVoid;

	if (pDfu->dfuStatus.bState == AT91C_DFU_appDETACH)
		pDfu->dfuStatus.bState = AT91C_DFU_appIDLE;
}

void AT91F_DfuDescStatusPollTimeOut(AT91S_TempoStatus status, void *pVoid)
{
	AT91PS_DfuDesc pDfu = (AT91PS_DfuDesc) pVoid;

	if (pDfu->dfuStatus.bState == AT91C_DFU_dfuDNBUSY)
		pDfu->dfuStatus.bState = AT91C_DFU_dfuDNLOAD_SYNC;


	else if (pDfu->dfuStatus.bState == AT91C_DFU_dfuMANIFEST) {
		/* Notify that we come back to dfuMANIFEST_SYNC after manifest */
		pDfu->manifestComplete = 1;
		/* We are supposed to be in dfuMANIFEST */
		if (pDfu->pDfuFDesc->bmAttributes & (1 << 2))
			pDfu->dfuStatus.bState = AT91C_DFU_dfuMANIFEST_SYNC;
		else
			pDfu->dfuStatus.bState = AT91C_DFU_dfuMANIFEST_WAIT_RESET;
	}
}

/* ============================================================================ */
/* Following static functions handle DFU state machine. They are called by      */
/* the class specific request: usbDfuDispatchRequest()                          */
/* 11 states are defined. Transitions are specified in the USB Device Firmware  */
/* Upgrade Specification, revision 1.0                                          */
/* ============================================================================ */

/* ***************************************************** */
/* **** dfuState0: Handle requests in appIDLE state **** */
/* ***************************************************** */
void AT91F_DfuDescState0(
	AT91PS_DfuDesc  pDfu,    /* DFU handler */
	AT91PS_UsbSetup pSetup)  /* Descriptor type */
{
	/* ==== Event: Receipt of the AT91C_DFU_DETACH request ==== */
	if (pSetup->bRequest == AT91C_DFU_DETACH) {
		/* wTimeOut should not contain a value larger than the value */
		/* specified in wDetachTimeout */
		if (pSetup->wValue > wDetachTimeOut(pDfu->pDfuFDesc))
			AT91F_DfuDescError(pDfu);
		else {
			/* NextState: appDetach */
			pDfu->dfuStatus.bState = AT91C_DFU_appDETACH;
			// Send a status
			AT91F_DfuDescSuccess(pDfu);
			// Start timer
			pDfu->tempo.Start(&(pDfu->tempo), pSetup->wValue, 0, AT91F_DfuDescDetachTimeout, (void *) pDfu);
		}
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATUS request ==== */
	else if (pSetup->bRequest == AT91C_DFU_GETSTATUS) {
		/* NextState: appIDDLE */
		pDfu->dfuStatus.bState = AT91C_DFU_appIDLE;
		/* Support of this request is optional in this state */
		AT91F_DfuDescGetStatus(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATE request ==== */
	else if (pSetup->bRequest == AT91C_DFU_GETSTATE) {
		/* NextState: appIDDLE */
		pDfu->dfuStatus.bState = AT91C_DFU_appIDLE;
		/* Support of this request is optional in this state */
		AT91F_DfuDescGetState(pDfu);
	}
	/* ==== Event: Receipt of any other DFU class-specific request ==== */
	else {
		/* NextState: appIDDLE */
		pDfu->dfuStatus.bState = AT91C_DFU_appIDLE;
		/* Device stalls the control pipe */
		AT91F_DfuDescError(pDfu);
	}
}

/* ******************************************************* */
/* **** dfuState1: Handle requests in appDETACH state **** */
/* ******************************************************* */
void AT91F_DfuDescState1(
	AT91PS_DfuDesc  pDfu,    /* DFU handler */
	AT91PS_UsbSetup pSetup)  /* Descriptor type */
{

	/* ==== Event: Receipt of the AT91C_DFU_GETSTATUS request ==== */
	if (pSetup->bRequest == AT91C_DFU_GETSTATUS) {
		/* NextState: appIDDLE */
		pDfu->dfuStatus.bState = AT91C_DFU_appDETACH;
		AT91F_DfuDescGetStatus(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATE request ==== */
	else if (pSetup->bRequest == AT91C_DFU_GETSTATE) {
		/* NextState: appIDDLE */
		pDfu->dfuStatus.bState = AT91C_DFU_appDETACH;
		AT91F_DfuDescGetState(pDfu);
	}
	/* ==== Event: Receipt of any other DFU class-specific request ==== */
	else {
		/* NextState: appIDDLE */
		pDfu->dfuStatus.bState = AT91C_DFU_appIDLE;
		/* Device stalls the control pipe */
		AT91F_DfuDescError(pDfu);
	}
}

/* ***************************************************** */
/* **** dfuState2: Handle requests in dfuIDLE state **** */
/* ***************************************************** */
void AT91F_DfuDescState2(
	AT91PS_DfuDesc  pDfu,    /* DFU handler */
	AT91PS_UsbSetup pSetup)  /* Descriptor type */
{
	register char           bRequest = pSetup->bRequest;
	register unsigned short wLength  = pSetup->wLength;
	register char           bitMask = bmAttributes(pDfu->pDfuFDesc);

	/* ==== Event: Receipt of the AT91C_DFU_DNLOAD request wLength > 0 and bitCanDnload = 1 ==== */
	if ((bRequest == AT91C_DFU_DNLOAD) && (wLength > 0) && (bitMask & 0x1)) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuDNLOAD_SYNC;
		pDfu->blockComplete = 0;
		pDfu->Download(pDfu, wLength, AT91F_DfuDescDownload);
		//AT91F_DfuDescSuccess(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_DNLOAD request wLength = 0 or bitCanDnload = 0 ==== */
	else if ((bRequest == AT91C_DFU_DNLOAD) && ((wLength == 0) || !(bitMask & 0x1))) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
		AT91F_DfuDescError(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_UPLOAD request and bitCanUpload = 1 ==== */
	else if ((bRequest == AT91C_DFU_UPLOAD) && (bitMask & 0x2)) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuUPLOAD_IDLE;
		pDfu->Upload(pDfu, wLength);
	}
	/* ==== Event: Receipt of the AT91C_DFU_UPLOAD request and bitCanUpload = 0 ==== */
	else if ((bRequest == AT91C_DFU_UPLOAD) && (!(bitMask & 0x2))) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
		AT91F_DfuDescError(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_ABORT request  ==== */
	else if (bRequest == AT91C_DFU_ABORT) {
		/* Does nothing */
		pDfu->dfuStatus.bState = AT91C_DFU_dfuIDLE;
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATUS request ==== */
	else if (bRequest == AT91C_DFU_GETSTATUS) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuIDLE;
		AT91F_DfuDescGetStatus(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATE request ==== */
	else if (bRequest == AT91C_DFU_GETSTATE) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuIDLE;
		AT91F_DfuDescGetState(pDfu);
	}
	/* ==== Event: Receipt of any other DFU class-specific request ==== */
	else {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
		AT91F_DfuDescError(pDfu);
	}
}

/* ************************************************************ */
/* **** dfuState3: Handle requests in dfuDNLOAD_SYNC state **** */
/* ************************************************************ */
void AT91F_DfuDescState3(
	AT91PS_DfuDesc  pDfu,    /* DFU handler */
	AT91PS_UsbSetup pSetup)  /* Descriptor type */
{
	register char         bRequest = pSetup->bRequest;
	unsigned int timeout;

	/* ==== Event: Receipt of the AT91C_DFU_GETSTATUS request. Bloc tx in progress ==== */
	if ( (bRequest == AT91C_DFU_GETSTATUS) && !(pDfu->blockComplete)) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuDNBUSY;
		AT91F_DfuDescGetStatus(pDfu);
		timeout = bwPollTimeout(&(pDfu->dfuStatus));
		pDfu->tempo.Start(&(pDfu->tempo), timeout, 0, AT91F_DfuDescStatusPollTimeOut, pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATUS request. Bloc tx complete ==== */
	//if ((bRequest == AT91C_DFU_GETSTATUS) && (!pMem || pMem->dnloadComplete)) {
	else if ( (bRequest == AT91C_DFU_GETSTATUS) && pDfu->blockComplete ) {
		// !!!!! To be modified !!!!!!
		pDfu->dfuStatus.bStatus = AT91C_DFU_OK;
		pDfu->dfuStatus.bState = AT91C_DFU_dfuDNLOAD_IDLE;
		AT91F_DfuDescGetStatus(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATE request ==== */
	else if (bRequest == AT91C_DFU_GETSTATE) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuDNLOAD_SYNC;
		AT91F_DfuDescGetState(pDfu);
	}
	/* ==== Event: Receipt of any other DFU class-specific request ==== */
	else {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
		AT91F_DfuDescError(pDfu);
	}
}

/* ******************************************************* */
/* **** dfuState4: Handle requests in dfuDNBUSY state **** */
/* ******************************************************* */
void AT91F_DfuDescState4(
	AT91PS_DfuDesc  pDfu,    /* DFU handler */
	AT91PS_UsbSetup pSetup)  /* Descriptor type */
{
	/* ==== Event: Receipt of any other DFU class-specific request ==== */
	pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
	AT91F_DfuDescError(pDfu);
}

/* ************************************************************ */
/* **** dfuState5: Handle requests in dfuDNLOAD_IDLE state **** */
/* ************************************************************ */
void AT91F_DfuDescState5(
	AT91PS_DfuDesc  pDfu,    /* DFU handler */
	AT91PS_UsbSetup pSetup)  /* Descriptor type */
{
	register char           bRequest = pSetup->bRequest;
	register unsigned short wLength  = pSetup->wLength;

	/* ==== Event: Receipt of the AT91C_DFU_DNLOAD request wLength > 0 ==== */
	if ((bRequest == AT91C_DFU_DNLOAD) && (wLength > 0)) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuDNLOAD_SYNC;
		pDfu->blockComplete = 0;
		pDfu->Download(pDfu, wLength, AT91F_DfuDescDownload);
		//AT91F_DfuDescSuccess(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_DNLOAD request wLength = 0 ==== */
	else if ((bRequest == AT91C_DFU_DNLOAD) && (wLength == 0)) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuMANIFEST_SYNC;
		AT91F_DfuDescSuccess(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_ABORT request  ==== */
	else if (bRequest == AT91C_DFU_ABORT) {
		/* Host is terminating teh current download transfer */
		pDfu->dfuStatus.bState = AT91C_DFU_dfuIDLE;
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATUS request ==== */
	else if (bRequest == AT91C_DFU_GETSTATUS) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuDNLOAD_IDLE;
		AT91F_DfuDescGetStatus(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATE request ==== */
	else if (bRequest == AT91C_DFU_GETSTATE) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuDNLOAD_IDLE;
		AT91F_DfuDescGetState(pDfu);
	}
	/* ==== Event: Receipt of any other DFU class-specific request ==== */
	else {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
		AT91F_DfuDescError(pDfu);
	}
}

/* ************************************************************** */
/* **** dfuState6: Handle requests in dfuMANIFEST_SYNC state **** */
/* ************************************************************** */
void AT91F_DfuDescState6(
	AT91PS_DfuDesc  pDfu,    /* DFU handler */
	AT91PS_UsbSetup pSetup)  /* Descriptor type */
{
	register char           bRequest = pSetup->bRequest;
	unsigned int timeout;

	/* ==== Event: Receipt of the AT91C_DFU_GETSTATUS request before manifest ==== */
	if ((bRequest == AT91C_DFU_GETSTATUS) && (pDfu->manifestComplete == 0)) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuMANIFEST;
		AT91F_DfuDescGetStatus(pDfu);
		timeout = bwPollTimeout(&(pDfu->dfuStatus));
		pDfu->tempo.Start(&(pDfu->tempo), timeout, 0, AT91F_DfuDescStatusPollTimeOut, pDfu);
	}

	/* ==== Event: Receipt of the AT91C_DFU_GETSTATUS request after manifest ==== */
	else if ((bRequest == AT91C_DFU_GETSTATUS) &&
	    (pDfu->manifestComplete == 1) &&
	    (pDfu->pDfuFDesc->bmAttributes & (1 << 2))) { // bitManifestationTolerant
		pDfu->manifestComplete = 0;
		pDfu->dfuStatus.bState = AT91C_DFU_dfuIDLE;
		AT91F_DfuDescGetStatus(pDfu);
	}

	/* ==== Event: Receipt of the AT91C_DFU_GETSTATE request ==== */
	else if (bRequest == AT91C_DFU_GETSTATE) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuMANIFEST_SYNC;
		AT91F_DfuDescGetState(pDfu);
	}
	/* ==== Event: Receipt of any other DFU class-specific request ==== */
	else {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
		AT91F_DfuDescError(pDfu);
	}
}

/* ************************************************************** */
/* **** dfuState7: Handle requests in dfuMANIFEST_SYNC state **** */
/* ************************************************************** */
void AT91F_DfuDescState7(
	AT91PS_DfuDesc  pDfu,    /* DFU handler */
	AT91PS_UsbSetup pSetup)  /* Descriptor type */
{
	/* ==== Event: Receipt of any other DFU class-specific request ==== */
	pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
	AT91F_DfuDescError(pDfu);
}

/* ******************************************************************** */
/* **** dfuState8: Handle requests in dfuMANIFEST_WAIT_RESET state **** */
/* ******************************************************************** */
void AT91F_DfuDescState8(
	AT91PS_DfuDesc  pDfu,    /* DFU handler */
	AT91PS_UsbSetup pSetup)  /* Descriptor type */
{
	/* ==== Event: Receipt of any other DFU class-specific request ==== */
	pDfu->dfuStatus.bState = AT91C_DFU_dfuMANIFEST_WAIT_RESET;
}

/* ************************************************************ */
/* **** dfuState9: Handle requests in dfuUPLOAD_IDLE state **** */
/* ************************************************************ */
void AT91F_DfuDescState9(
	AT91PS_DfuDesc  pDfu,    /* DFU handler */
	AT91PS_UsbSetup pSetup)  /* Descriptor type */
{
	register char           bRequest = pSetup->bRequest;
	register unsigned short wLength  = pSetup->wLength;
	//register USBDfuMem      *pMem    = pDfu->pMem;

	/* ==== Event: Receipt of the AT91C_DFU_UPLOAD request wLength > 0 ==== */
	if ((bRequest == AT91C_DFU_UPLOAD) && (wLength > 0)) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuUPLOAD_IDLE;
		pDfu->Upload(pDfu, wLength);
	}
	/* ==== Event: Receipt of the AT91C_DFU_ABORT request  ==== */
	else if (bRequest == AT91C_DFU_ABORT) {
		/* Host is terminating teh current download transfer */
		pDfu->dfuStatus.bState = AT91C_DFU_dfuIDLE;
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATUS request ==== */
	else if (bRequest == AT91C_DFU_GETSTATUS) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuUPLOAD_IDLE;
		AT91F_DfuDescGetStatus(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATE request ==== */
	else if (bRequest == AT91C_DFU_GETSTATE) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuUPLOAD_IDLE;
		AT91F_DfuDescGetState(pDfu);
	}
	/* ==== Event: Receipt of any other DFU class-specific request ==== */
	else {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
		AT91F_DfuDescError(pDfu);
	}
}

/* ******************************************************* */
/* **** dfuState10: Handle requests in dfuERROR state **** */
/* ******************************************************* */
void AT91F_DfuDescState10(
	AT91PS_DfuDesc  pDfu,    /* DFU handler */
	AT91PS_UsbSetup pSetup)  /* Descriptor type */
{
	register char           bRequest = pSetup->bRequest;

	/* ==== Event: Receipt of the AT91C_DFU_GETSTATUS request ==== */
	if (bRequest == AT91C_DFU_GETSTATUS) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
		AT91F_DfuDescGetStatus(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_GETSTATE request ==== */
	else if (bRequest == AT91C_DFU_GETSTATE) {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
		AT91F_DfuDescGetState(pDfu);
	}
	/* ==== Event: Receipt of the AT91C_DFU_CLRSTATUS request  ==== */
	else if (bRequest == AT91C_DFU_CLRSTATUS) {
		pDfu->dfuStatus.bStatus = AT91C_DFU_OK;
		pDfu->dfuStatus.bState  = AT91C_DFU_dfuIDLE;
	}
	/* ==== Event: Receipt of any other DFU class-specific request ==== */
	else {
		pDfu->dfuStatus.bState = AT91C_DFU_dfuERROR;
		AT91F_DfuDescError(pDfu);
	}
}

/* ============================================================================ */
/* ============================================================================ */
