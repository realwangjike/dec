//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : obj_udpdesc.h
//* Object              : USB peripheral library.
//*
//* 1.0 Sept 3rd 01 ODi : Creation
//*----------------------------------------------------------------------------
#ifndef obj_usbdesc_h
#define obj_usbdesc_h

#include 	"periph/udp/lib_udp.h"

/* *********************************************************************** */
/* Following definitions corresponds to the USB specification Rev. 1.1     */
/* They are used during the USB enumeration                                */
/* *********************************************************************** */

/* USB standard request code */
#define USB_GET_STATUS           0
#define USB_CLEAR_FEATURE        1
#define USB_SET_FEATURE          3
#define USB_SET_ADDRESS          5
#define USB_GET_DESCRIPTOR       6
#define USB_SET_DESCRIPTOR       7
#define USB_GET_CONFIGURATION    8
#define USB_SET_CONFIGURATION    9
#define USB_GET_INTERFACE       10
#define USB_SET_INTERFACE       11
#define USB_SYNCH_FRAME         12

/* USB descriptor types */
#define USB_DEVICE               1
#define USB_CONFIGURATION        2
#define USB_STRING               3
#define USB_INTERFACE            4
#define USB_ENDPOINT             5

/* USB standard feature selectors */
#define USB_DEVICE_REMOTE_WAKEUP 1
#define USB_ENDPOINT_HALT        0
/*---------------------------*/
/* USB Setup Data            */
/*---------------------------*/
typedef struct _AT91S_UsbSetup {
	char  bmRequestType;      /* Characteristics of request */
	char  bRequest;           /* Specific request */
	short wValue;             /* Word-sized field */
	short wIndex;             /* Word-sized field; used to pass an index or an offset */
	short wLength;            /* Number of bytes to transfert */
} AT91S_UsbSetup, *AT91PS_UsbSetup;

/*-------------------------------------------*/
/* USB Standard device descriptor            */
/*-------------------------------------------*/
typedef __packed struct _AT91S_UsbDevice {
	char  bLength;            /* Size of this descriptor */
	char  bDescriptorType;    /* DEVICE descriptor type */
	short bcdUSB;             /* USB Specification release Number */
	char  bDeviceClass;       /* Class code (assigned by the USB) */
	char  bDeviceSubClass;    /* Subclass code (assigned by the USB) */
	char  bDeviceProtocol;    /* Protool code (assigned by the USB) */
	char  bMaxPacketSize0;    /* Maximum packet size for endpoint 0 */
	short idVendor;           /* Vendor ID */
	short idProduct;          /* Product ID */
	short bcdDevice;          /* Device release number */
	char  iManufacturer;      /* Index of string descriptor describing manufacturer*/
	char  iProduct;           /* Index of string descriptor describing the product */
	char  iSerialNumber;      /* Index of string descriptor describing the device's serial number */
	char  bNumConfigurations; /* Number of possible configurations */
} AT91S_UsbDevice, *AT91PS_UsbDevice;


/*-------------------------------------------*/
/* USB Standard configuration descriptor     */
/*-------------------------------------------*/
typedef __packed struct _AT91S_UsbConfiguration {
	char  bLength;            /* Size of this descriptor */
	char  bDescriptorType;    /* CONFIGURATION descriptor type */
	short wTotalLength;       /* Total length of data returned for this configuration */
	char bNumInterfaces;      /* Number of interfaces supported by this configuration */
	char bConfigurationValue; /* Value to use as an argument to the set configuration() request */
	char iConfiguration;      /* Index of string descriptor describing this configuration */
	char bmAttributes;        /* Configuration characteristics */
	char MaxPower;            /* Maximum Power consumption */
} AT91S_UsbConfiguration, *AT91PS_UsbConfiguration;

__inline u_short wTotalLength(char const *pCfg) { return ((pCfg[3] << 8) | pCfg[2]); }

/*---------------------------------------*/
/* USB Standard interface descriptor     */
/*---------------------------------------*/
typedef __packed struct _AT91S_UsbInterface {
	char  bLength;            /* Size of this descriptor */
	char  bDescriptorType;    /* INTERFACE descriptor type */
	char  bInterfaceNumber;   /* Number of interface */
	char  bAlternateSetting;  /* Number of interfaces supported by this configuration */
	char  bNumEndpoints;      /* Number of endpoints used by this interface */
	char  bInterfaceClass;    /* Class code */
	char  bInterfaceSubClass; /* Subclass code */
	char  bInterfaceProtocol; /* Protocol code */
	char  iInterface;         /* Index of string descriptor describing this interface */
} AT91S_UsbInterface, *AT91PS_UsbInterface;

/*--------------------------------------*/
/* USB Standard endpoint descriptor     */
/*--------------------------------------*/
typedef __packed struct _AT91S_UsbEndpoint {
	char  bLength;            /* Size of this descriptor */
	char  bDescriptorType;    /* ENDPÖINT descriptor type */
	char  bEndpointAddress;   /* Address of the endpoint on the USB device described by this descriptor */
	char  bmAttributes;       /* Bitmap */
	short wMaxPacketSize;     /* Maximum packet size this endpoint is capable of sending or receiving */
	char  bInterval;          /* Interval for polling endpoint for data transfers */
} AT91S_UsbEndpoint, *AT91PS_UsbEndpoint;

__inline char           bEndpointAddress(char const *pEp)  { return (pEp[2] & 0x0F); }
__inline unsigned short wMaxPacketSize  (char const *pEp)  { return ((pEp[5] << 8) | pEp[4]); }

/*------------------------------------------*/
/* USB Standard UNICODE string descriptor  */
/*-----------------------------------------*/
typedef __packed struct _AT91S_UsbString {
	char  bLength;            /* Size of this descriptor */
	char  bDescriptorType;    /* STRING descriptor type */
	char  *bString;           /* UNICODE encoded string */
} AT91S_UsbString, *AT91PS_UsbString;




/* *********************************************************************** */
/* Following structure must be defined by the application                  */
/* Each pointer to function corresponds to standard request from the host  */
/* defined by the USB spec. Rev. 1.1                                       */
/*                                                                         */
/* If the request is not supported, the field must be set to 0. A STALL    */
/* handshake will be initialized with the host.                            */
/* *********************************************************************** */

typedef struct _AT91S_UsbDesc {

/* Following function is the entry point for the application */
	void (*DispatchRequest)    (
		struct _AT91S_UsbDesc *pUsbDesc, /* Pointer to this structure */
		AT91PS_UsbSetup  pSetup);           /* Descriptor type */
/* ======================================================================= */
/* Following functions are called by usbDispatchRequest() if a class or a  */
/* vendor request has been detected                                        */
/* ======================================================================= */
	void (*DispatchClassRequest)    (
		AT91PS_UsbSetup  pSetup,      /* Descriptor type */
		void *pPrivateClassData);  /* Pointer to private datas */
	void (*DispatchVendorRequest) (
		AT91PS_UsbSetup  pSetup,      /* Descriptor type */
		void *pPrivateVendorData); /* Pointer to private datas */

/* ======================================================================= */
/* Following functions are called after the request has been checked       */
/* according to the USB macro status                                       */
/* ======================================================================= */
	void (*ClearFeature) (
		char    type,         /* Descriptor type */
		char    index,        /* Descriptor index */
		short   feature,      /* Feature */
		void    *pPrvtStdDt); /* Pointer to private Standard data */

	void (*GetConfiguration)(
		void    *pPrvtStdDt); /* Pointer to private Standard data */
	void (*GetDescriptor) (
		char    type,         /* Descriptor type */
		char    index,        /* Descriptor index */
		short   langID,       /* Language ID */
		short   length,       /* Desriptor length */
		void    *pPrvtStdDt); /* Pointer to private Standard data */
	void (*GetInterface) (
		short   interface,    /* Interface index */
		void    *pPrvtStdDt); /* Pointer to private Standard data */
	void (*GetStatus) (
		char    recipient,    /* device, interface, endpoint */
		short   index,        /* interface or endpoint index */
		void    *pPrvtStdDt); /* Pointer to private Standard data */
	void (*SetAddress) (
		char    address,      /* device, interface, endpoint */
		void    *pPrvtStdDt); /* Pointer to private Standard data */
	void (*SetConfiguration) (
		short   cfgValue,     /* Configuration value */
		void    *pPrvtStdDt); /* Pointer to private Standard data */
	void (*SetDescriptor) (
		char    type,         /* Descriptor type */
		char    index,        /* Descriptor index */
		short   langID,       /* Language ID */
		short   length,       /* Desriptor length */
		void    *pPrvtStdDt); /* Pointer to private Standard data */
	void (*SetFeature) (
		char    recipient,    /* device, interface, endpoint */
		short   index,        /* interface or endpoint index */
		short   feature,      /* Feature selector */
		void    *pPrvtStdDt); /* Pointer to private Standard data */
	void (*SetInterface) (
		short   interface,    /* Interface index */
		short   setting,      /* Alternative setting */
		void    *pPrvtStdDt); /* Pointer to private Standard data */
	void (*SynchFrame) (
		short   endpoint,     /* Endpoint index */
		void    *pPrvtStdDt); /* Pointer to private Standard data */
/* ======================================================================= */
/* Following pointers are used by dispatchClassRequest()                   */
/* and dispatchVendorRequest()                                             */
/* ======================================================================= */
	void *pPrivateStandardData;
	void *pPrivateClassData;
	void *pPrivateVendorData;

/* Control Endpoint definition */
	AT91PS_UDP     pUdp;
	char           epId;
} AT91S_UsbDesc, *AT91PS_UsbDesc;

typedef AT91PS_UsbDesc (*AT91PF_OpenUsbDesc) (AT91PS_UsbDesc, AT91PS_UDP,	char);

typedef void (*AT91PF_DispatchRequest)       (AT91PS_UsbDesc, AT91PS_UsbSetup);
typedef void (*AT91PF_DispatchClassRequest)  (AT91PS_UsbSetup, void *);
typedef void (*AT91PF_DispatchVendorRequest) (AT91PS_UsbSetup, void *);
typedef void (*AT91PF_ClearFeature)          (char, char, short, void *);
typedef void (*AT91PF_GetConfiguration)      (void *);
typedef void (*AT91PF_GetDescriptor)         (char, char, short, short, void *);
typedef void (*AT91PF_GetInterface)          (short, void *);
typedef void (*AT91PF_GetStatus)             (char, short, void *);
typedef void (*AT91PF_SetAddress)            (char, void *);
typedef void (*AT91PF_SetConfiguration)      (short, void *);
typedef void (*AT91PF_SetDescriptor)         (char, char, short, short, void *);
typedef void (*AT91PF_SetFeature)            (char, short, short, void *);
typedef void (*AT91PF_SetInterface)          (short, short, void *);
typedef void (*AT91PF_SynchFrame)            (short, void *);

/* *********************************************************************** */
/* Following functions are define in obj_usbdesc.c. They handle standard  */
/* request in order to match with USB spec. Rev. 1.1. Once the request has */
/* been checked according to the USB status, the request is transmitted to */
/* the application calling the application request handler.                */
/* *********************************************************************** */


/* ======================================================================= */
/* Following functions are used when a Setup packet has been sent to the   */
/* device. According to the request, they call the corresponding functions */
/* ======================================================================= */
extern AT91PS_UsbDesc AT91F_OpenUsbDesc(
	AT91PS_UsbDesc  pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UDP      pUdp,      /* Uregister structure */
	char            epId);     /* endpoint number */

/* usbDescDispatchRequest handle standart request or call */
/* USBDesc->dispatchVendorRequest() or                    */
/* USBDesc->dispatchClassRequest()                        */
extern void AT91F_UsbDescDispatchRequest(
	AT91PS_UsbDesc const pDesc,  /* pointer to application request handlers */
	AT91PS_UsbSetup  pSetup); /* Descriptor type */


/* ======================================================================= */
/* Following functions handle standard request they are called by the      */
/* application through usbDescDispatchRequest ().                          */
/* If this is a valid request, they transmit the request to the application*/
/* layer                                                                   */
/* ======================================================================= */

extern void AT91F_UsbDescClearFeature(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup  pSetup);        /* Descriptor type */
extern void AT91F_UsbDescGetConfiguration(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup  pSetup);        /* Descriptor type */
extern void AT91F_UsbDescGetDescriptor(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup  pSetup);        /* Descriptor type */
extern void AT91F_UsbDescGetInterface(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup  pSetup);        /* Descriptor type */
extern void AT91F_UsbDescGetStatus(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup  pSetup);        /* Descriptor type */
extern void AT91F_UsbDescSetAddress(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup  pSetup);        /* Descriptor type */
extern void AT91F_UsbDescSetConfiguration(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup  pSetup);        /* Descriptor type */
extern void AT91F_UsbDescSetDescriptor(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup  pSetup);        /* Descriptor type */
extern void AT91F_UsbDescSetFeature(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup  pSetup);        /* Descriptor type */
extern void AT91F_UsbDescSetInterface(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup  pSetup);        /* Descriptor type */
extern void AT91F_UsbDescSynchFrame(
	AT91PS_UsbDesc const pDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbSetup  pSetup);        /* Descriptor type */

// Small utility function to configure the UDP endpoint
// according to the endpoint descriptor
extern void AT91F_UdpConfigure(
	AT91PS_UsbDesc const pUsbDesc,  /* pointer to a AT91S_UsbDesc structure */
	AT91PS_UsbEndpoint const pEp);

/* ************************************ */
/* usbSendStatus                        */
/*                                      */
/* Arguments:                           */
/*     pPipe: pointer to default pipe   */
/*            handler                   */
/* Return:                              */
/*     Nothing                          */
/* Description:                         */
/*     Acknowledge a control write tx.  */
/*     Send a sero length packet        */
/* ************************************ */
//__inline void AT91F_UsbDescSendStatus(AT91PS_Pipe pPipe)
//{
//	/* Send a zero length packet */
//	pPipe->Write(pPipe, 0, 0);
//}

#endif /* obj_usbdesc_h */

