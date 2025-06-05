//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : obj_udpdesc.c
//* Object              : USB peripheral library.
//*
//* 1.0 07/23/01 ODi    : Creation
//*----------------------------------------------------------------------------
#include "services/obj_usbdesc.h"



/* ************************************ */
/* AT91F_OpenUsbDesc                    */
/*                                      */
/* Arguments:                           */
/* Return:                              */
/*     Pointer toAT91PS_UsbDesc object  */
/* Description:                         */
/* ************************************ */
AT91PS_UsbDesc AT91F_OpenUsbDesc(
	AT91PS_UsbDesc  pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UDP      pUdp,      /* Uregister structure */
	char            epId)      /* endpoint number */
{
	pUsbDesc->DispatchRequest = AT91F_UsbDescDispatchRequest;
	//pUsbDesc->DispatchClassRequest  = (AT91PF_DispatchClassRequest)  0;
	//pUsbDesc->DispatchVendorRequest = (AT91PF_DispatchVendorRequest) 0;
	//pUsbDesc->ClearFeature          = (AT91PF_ClearFeature)          0;
	//pUsbDesc->GetConfiguration      = (AT91PF_GetConfiguration)      0;
	//pUsbDesc->GetDescriptor         = (AT91PF_GetDescriptor)         0;
	//pUsbDesc->GetInterface          = (AT91PF_GetInterface)          0;
	//pUsbDesc->GetStatus             = (AT91PF_GetStatus)             0;
	//pUsbDesc->SetAddress            = (AT91PF_SetAddress)            0;
	//pUsbDesc->SetConfiguration      = (AT91PF_SetConfiguration)      0;
	//pUsbDesc->SetDescriptor         = (AT91PF_SetDescriptor)         0;
	//pUsbDesc->SetFeature            = (AT91PF_SetFeature)            0;
	//pUsbDesc->SetInterface          = (AT91PF_SetInterface)          0;
	//pUsbDesc->SynchFrame            = (AT91PF_SynchFrame)            0;

	pUsbDesc->pUdp = pUdp;
	pUsbDesc->epId = epId;
	return pUsbDesc;
}

/* ************************************ */
/* usbDescDispatchRequest               */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request handle standard     */
/*     request or call                  */
/*     usbDescDispatchClassRequest      */
/*     usbDescDispatchVendorRequest     */
/* ************************************ */
void AT91F_UsbDescDispatchRequest(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup) /* USB setup data Cf USB spec rev 1.1 */
{
	switch ((pSetup->bmRequestType & (0x60)) >> 5) {
	case 0: /* Standard request */
		switch (pSetup->bRequest) {
		case USB_CLEAR_FEATURE:     AT91F_UsbDescClearFeature    (pDesc, pSetup); break;
		case USB_GET_CONFIGURATION: AT91F_UsbDescGetConfiguration(pDesc, pSetup); break;
		case USB_GET_DESCRIPTOR:    AT91F_UsbDescGetDescriptor   (pDesc, pSetup); break;
		case USB_GET_INTERFACE:     AT91F_UsbDescGetInterface    (pDesc, pSetup); break;
		case USB_GET_STATUS:        AT91F_UsbDescGetStatus       (pDesc, pSetup); break;
		case USB_SET_ADDRESS:       AT91F_UsbDescSetAddress      (pDesc, pSetup); break;
		case USB_SET_CONFIGURATION: AT91F_UsbDescSetConfiguration(pDesc, pSetup); break;
		case USB_SET_DESCRIPTOR:    AT91F_UsbDescSetDescriptor   (pDesc, pSetup); break;
		case USB_SET_FEATURE:       AT91F_UsbDescSetFeature      (pDesc, pSetup); break;
		case USB_SET_INTERFACE:     AT91F_UsbDescSetInterface    (pDesc, pSetup); break;
		case USB_SYNCH_FRAME:       AT91F_UsbDescSynchFrame      (pDesc, pSetup); break;
		default:                    AT91F_UdpEpStall             (pDesc->pUdp, pDesc->epId);                                      break;
		}
		break;
	case 1: /* Class request */
		if (pDesc->DispatchClassRequest)
			pDesc->DispatchClassRequest(pSetup, pDesc->pPrivateClassData);
		break;
	case 2: /* Vendor request */
		if (pDesc->DispatchVendorRequest)
			pDesc->DispatchVendorRequest(pSetup, pDesc->pPrivateVendorData);
		break;
	default: /* Reserved */
		AT91F_UdpEpStall(pDesc->pUdp, pDesc->epId);
		break;
	}
	return;
}

/* ************************************ */
/* AT91F_UsbDescClearFeature            */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request is used to clear or */
/*     disable a specific feature       */
/* ************************************ */
void AT91F_UsbDescClearFeature(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup)    /* USB setup data Cf USB spec rev 1.1 */
{
	u_int usbState = AT91F_UdpGetState(pUsbDesc->pUdp);
	u_int recipient = (pSetup->bmRequestType & 0x1F);

	/* Check request arguments */
	if (recipient > 2 || pSetup->wLength != 0)
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);

	/* ================ Configured state ================= */
	else if ( (usbState & UDP_CONFG) ) {
		/* If an application handler exists, transmit the request */
		if (pUsbDesc->ClearFeature)
			pUsbDesc->ClearFeature(recipient, pSetup->wIndex, pSetup->wValue, pUsbDesc->pPrivateStandardData);
		else
			AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	}
	/* =================== Address state ================= */
	else if ( (usbState & UDP_FADDEN) ) {
		/* This request is valid in Address State only when reference endpoint 0 */
		if (recipient != 0 && pSetup->wIndex != 0)
			AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
		else {
		/* If an application handler exists, transmit the request */
			if (pUsbDesc->ClearFeature )
				pUsbDesc->ClearFeature(recipient, pSetup->wIndex, pSetup->wValue, pUsbDesc->pPrivateStandardData);
			else
				AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
		}
	}
	/* =================== Default state ================= */
	else
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
}

/* ************************************ */
/* AT91F_UsbDescGetConfiguration        */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request returns the current */
/*     device configuration value       */
/* ************************************ */
void AT91F_UsbDescGetConfiguration(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup)    /* USB setup data Cf USB spec rev 1.1 */
{
	u_int usbState = AT91F_UdpGetState(pUsbDesc->pUdp);

	/* Check request arguments */
	if (pSetup->wValue != 0 || pSetup->wIndex != 0 || pSetup->wLength != 1)
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);

	/* ================ Configured state ================= */
	else if ( (usbState & UDP_CONFG) ) {
		/* If an application handler exists, transmit the request */
		if (pUsbDesc->GetConfiguration )
			pUsbDesc->GetConfiguration(pUsbDesc->pPrivateStandardData);
		else
			AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	}
	/* =================== Address state ================= */
	else if ( (usbState & UDP_FADDEN) ) {
		/* Return a 0 value */
		AT91F_UdpEpWrite(pUsbDesc->pUdp, pUsbDesc->epId, "\0x00", 1);
		AT91F_UdpEpEow(pUsbDesc->pUdp, pUsbDesc->epId);
	}
	/* =================== Default state ================= */
	else
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
}

/* ************************************ */
/* AT91F_UsbDescGetDescriptor           */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request returns the         */
/*     specified descriptor if the      */
/*     descriptor exists                */
/* ************************************ */
void AT91F_UsbDescGetDescriptor(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup)    /* USB setup data Cf USB spec rev 1.1 */
{
	/* This request is valid in all states */
	if (pUsbDesc->GetDescriptor)
		pUsbDesc->GetDescriptor (
			((pSetup->wValue) >> 8),   /* Descriptor type */
			((pSetup->wValue) & 0xFF), /* Descriptor Index */
			pSetup->wIndex,            /* Language ID */
			pSetup->wLength,           /* Descripotr Length */
			pUsbDesc->pPrivateStandardData);
	else
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);

}

/* ************************************ */
/* AT91F_UsbDescGetInterface            */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request returns the         */
/*     selected alternate setting for   */
/*     the specified interface          */
/* ************************************ */
void AT91F_UsbDescGetInterface(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup)    /* USB setup data Cf USB spec rev 1.1 */
{
	u_int usbState = AT91F_UdpGetState(pUsbDesc->pUdp);

	/* Check request arguments */
	if (pSetup->wValue != 0 || pSetup->wLength != 1)
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);

	/* ================ Configured state ================= */
	else if ( (usbState & UDP_CONFG) ) {
		/* If an application handler exists, transmit the request */
		if (pUsbDesc->GetInterface)
			pUsbDesc->GetInterface(pSetup->wIndex, pUsbDesc->pPrivateStandardData);
		else
			AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	}
	/* =================== Address state ================= */
	else if ( (usbState & UDP_FADDEN) )
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	/* =================== Default state ================= */
	else
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
}

/* ************************************ */
/* AT91F_UsbDescGetStatus               */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request returns status for  */
/*     the specified endpoint           */
/* ************************************ */
void AT91F_UsbDescGetStatus(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup)    /* USB setup data Cf USB spec rev 1.1 */
{
	u_int usbState  = AT91F_UdpGetState(pUsbDesc->pUdp);
	u_int recipient = (pSetup->bmRequestType & 0x1F);

	/* Check request arguments */
	if (recipient > 2 || pSetup->wValue != 0 || pSetup->wLength != 2)
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	else if (recipient == 0 && pSetup->wIndex != 0)
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);

	/* ================ Configured state ================= */
	else if ( (usbState & UDP_CONFG) ) {
		if (pUsbDesc->GetStatus)
			pUsbDesc->GetStatus(recipient, pSetup->wIndex, pUsbDesc->pPrivateStandardData);
		else
			AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	}
	/* =================== Address state ================= */
	else if ( (usbState & UDP_FADDEN) ) {
		if (pSetup->wIndex != 0 || !(pUsbDesc->GetStatus))
			AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
		else
			pUsbDesc->GetStatus(recipient, pSetup->wIndex, pUsbDesc->pPrivateStandardData);
	}
	/* =================== Default state ================= */
	else
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
}

/* ************************************ */
/* AT91F_UsbDescSetAddress              */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request sets the device     */
/*     address for all device accesses  */
/* ************************************ */
void AT91F_UsbDescSetAddress(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup)    /* USB setup data Cf USB spec rev 1.1 */
{
	u_int usbState  = AT91F_UdpGetState(pUsbDesc->pUdp);

	/* Check request arguments */
	if ((pSetup->wValue >= 127) || pSetup->wIndex != 0 || pSetup->wLength != 0)
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);

	/* ================ Configured state ================= */
	else if ( (usbState & UDP_CONFG) )
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	/* ============ Address or Default state ============= */
	else {
		/* Enter the address state if the value != 0 */
		//AT91F_UdpEpEow(pUsbDesc->pUdp, pUsbDesc->epId);
		// Wait for TX Complete
		if (pUsbDesc->SetAddress)
			pUsbDesc->SetAddress(pSetup->wValue, pUsbDesc->pPrivateStandardData);
	}
}

/* ************************************ */
/* AT91F_UsbDescSetConfiguration        */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request sets the device     */
/*     configuration                    */
/* ************************************ */
void AT91F_UsbDescSetConfiguration(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup)    /* USB setup data Cf USB spec rev 1.1 */
{
	u_int usbState  = AT91F_UdpGetState(pUsbDesc->pUdp);

	/* Check request arguments */
	if ((pSetup->wValue & 0xFF00) || pSetup->wIndex != 0 || pSetup->wLength != 0)
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);

	/* ================ Configured state ================= */
	else if ( (usbState & UDP_CONFG) ) {
		if (pSetup->wValue == 0) {
			/* Device enters Address State */
			AT91F_UdpSetState(pUsbDesc->pUdp, UDP_FADDEN);
			AT91F_UdpEpEow(pUsbDesc->pUdp, pUsbDesc->epId);
		}
		else {
			if (pUsbDesc->SetConfiguration)
				pUsbDesc->SetConfiguration(pSetup->wValue, pUsbDesc->pPrivateStandardData);
			else
				AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
		}
	}
	/* =================== Address state ================= */
	else if ( (usbState & UDP_FADDEN) ) {
		if (pSetup->wValue == 0)
			/* Device remains in Address State */
			AT91F_UdpEpEow(pUsbDesc->pUdp, pUsbDesc->epId);
		else {
			if (pUsbDesc->SetConfiguration)
				pUsbDesc->SetConfiguration(pSetup->wValue, pUsbDesc->pPrivateStandardData);
			else
				AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
		}
	}
	/* =================== Default state ================= */
	else
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
}

/* ************************************ */
/* AT91F_UsbDescSetDescriptor           */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request may be used to      */
/*     update existing descripotrs or   */
/*     new descriptors may be added     */
/* ************************************ */
void AT91F_UsbDescSetDescriptor(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup)    /* USB setup data Cf USB spec rev 1.1 */
{
	u_int usbState  = AT91F_UdpGetState(pUsbDesc->pUdp);

	/* ================ Configured or Address state ================= */
	if ( ((usbState & UDP_CONFG) || (usbState & UDP_FADDEN))) {
		if (pUsbDesc->SetDescriptor)
			pUsbDesc->SetDescriptor (
				(pSetup->wValue >> 8),   /* Descriptor type */
				(pSetup->wValue & 0xFF), /* Descriptor index */
				pSetup->wIndex,          /* Language ID */
				pSetup->wLength,         /* Desriptor length */
				pUsbDesc->pPrivateStandardData);
		else
			AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	}
	/* =================== Default state ================= */
	else
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);

}

/* ************************************ */
/* AT91F_UsbDescSetFeature              */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request is used to set or   */
/*     enable a specific feature        */
/* ************************************ */
void AT91F_UsbDescSetFeature(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup)    /* USB setup data Cf USB spec rev 1.1 */
{
	u_int usbState = AT91F_UdpGetState(pUsbDesc->pUdp);
	u_int recipient = (pSetup->bmRequestType & 0x1F);

	/* Check request arguments */
	if (recipient > 2 || pSetup->wLength != 0)
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);

	/* ================ Configured state ================= */
	else if ( (usbState & UDP_CONFG) ) {
		if (pUsbDesc->SetFeature)
			pUsbDesc->SetFeature (recipient, pSetup->wIndex, pSetup->wValue, pUsbDesc->pPrivateStandardData);
		else
			AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	}
	/* =================== Address state ================= */
	else if ( (usbState & UDP_FADDEN) ) {
		/* This request is valid in Address State only when reference endpoint 0 */
		if (recipient != 2 || pSetup->wIndex != 0)
			AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
		else {
			if (pUsbDesc->SetFeature)
				pUsbDesc->SetFeature (recipient, pSetup->wIndex, pSetup->wValue, pUsbDesc->pPrivateStandardData);
			else
				AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
		}
	}
	/* =================== Default state ================= */
	else
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
}

/* ************************************ */
/* AT91F_UsbDescSetInterface            */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request allows the host to  */
/*     select an alternate setting for  */
/*     the specified interface          */
/* ************************************ */
void AT91F_UsbDescSetInterface(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup)    /* USB setup data Cf USB spec rev 1.1 */
{
	u_int usbState = AT91F_UdpGetState(pUsbDesc->pUdp);

	/* Check request arguments */
	if (pSetup->wLength != 0)
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);

	/* ================ Configured state ================= */
	else if ( (usbState & UDP_CONFG) ) {
		if (pUsbDesc->SetInterface)
			pUsbDesc->SetInterface (pSetup->wIndex, pSetup->wValue, pUsbDesc->pPrivateStandardData);
		else
			AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	}
	/* =================== Address state ================= */
	else if ( (usbState & UDP_FADDEN) )
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	/* =================== Default state ================= */
	else
		/* Device behavior not specified */
		AT91F_UdpEpEow(pUsbDesc->pUdp, pUsbDesc->epId);
}

/* ************************************ */
/* AT91F_UsbDescSynchFrame              */
/*                                      */
/* Arguments:                           */
/*     pSetup: pointer to setup datas   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     This request is used to set and  */
/*     then report an ep's synch.frame. */
/* ************************************ */
void AT91F_UsbDescSynchFrame(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup      pSetup)    /* USB setup data Cf USB spec rev 1.1 */
{
	u_int usbState = AT91F_UdpGetState(pUsbDesc->pUdp);

	/* Check request arguments */
	if (pSetup->wValue != 0 || pSetup->wLength != 2)
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);

	/* ================ Configured state ================= */
	else if ( (usbState & UDP_CONFG) ) {
		if (pUsbDesc->SynchFrame)
			pUsbDesc->SynchFrame (pSetup->wIndex, pUsbDesc->pPrivateStandardData);
		else
			AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	}
	/* =================== Address state ================= */
	else if ( (usbState & UDP_FADDEN) )
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
	/* =================== Default state ================= */
	else
		/* Device behavior not specified */
		AT91F_UdpEpStall(pUsbDesc->pUdp, pUsbDesc->epId);
}

/* ************************************ */
/* AT91F_UdpConfigure                   */
/*                                      */
/* Arguments:                           */
/*     pEp:   pointer to endpoint       */
/*            standard USB descriptor   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     Configure USB endpoints according*/
/*     to the endpoint descriptor       */
/* ************************************ */
void AT91F_UdpConfigure(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbEndpoint const pEp)
{
	if (pEp->bEndpointAddress & (1 << 7)) {
		/* IN endpoint */
		switch (pEp->bmAttributes & 0x03) {
		case 0:  AT91F_UdpEpConfigure(pUsbDesc->pUdp, pUsbDesc->epId, UDP_EPDS | UDP_CONTROL);        break;
		case 1:  AT91F_UdpEpConfigure(pUsbDesc->pUdp, pUsbDesc->epId, UDP_EPDS | UDP_ISOCHRONOUS_IN); break;
		case 2:  AT91F_UdpEpConfigure(pUsbDesc->pUdp, pUsbDesc->epId, UDP_EPDS | UDP_BULK_IN);        break;
		default: AT91F_UdpEpConfigure(pUsbDesc->pUdp, pUsbDesc->epId, UDP_EPDS | UDP_INTERRUPT_IN);   break;
		}
	}
	else {
		/* OUT endpoint */
		switch (pEp->bmAttributes & 0x03) {
		case 0:  AT91F_UdpEpConfigure(pUsbDesc->pUdp, pUsbDesc->epId, UDP_EPDS | UDP_CONTROL);         break;
		case 1:  AT91F_UdpEpConfigure(pUsbDesc->pUdp, pUsbDesc->epId, UDP_EPDS | UDP_ISOCHRONOUS_OUT); break;
		case 2:  AT91F_UdpEpConfigure(pUsbDesc->pUdp, pUsbDesc->epId, UDP_EPDS | UDP_BULK_OUT);        break;
		default: AT91F_UdpEpConfigure(pUsbDesc->pUdp, pUsbDesc->epId, UDP_EPDS | UDP_INTERRUPT_OUT);   break;
		}
	}
}
