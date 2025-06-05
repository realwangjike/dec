//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : obj_svcdataflash
//* Object              : Dataflash definition service
//*
//* 1.0 26/01/02 	    : HI Creation
//* 1.2 08/10/02 FB HI  : New structure
//*----------------------------------------------------------------------------
#ifndef obj_svcdataflash_h
#define obj_svcdataflash_h

#include "appli/includes/dataflash.h"

/************************************************************************************************************************/
typedef u_int AT91S_SvcDataFlashStatus;

typedef struct _AT91PS_SvcDataFlash {
	void 					 (*Handler)						(struct _AT91PS_SvcDataFlash *, u_int );
	AT91S_SvcDataFlashStatus (*Status)  					(struct _AT91S_DataflashDesc *);
	void					 (*AbortCommand)  				(struct _AT91S_DataflashDesc *);

	AT91S_SvcDataFlashStatus (*PageRead)   					(struct _AT91PS_SvcDataFlash *, u_int, u_char *, int);
	AT91S_SvcDataFlashStatus (*ContinuousRead)   			(struct _AT91PS_SvcDataFlash *, int, u_char *, int);
	AT91S_SvcDataFlashStatus (*ReadBuffer)   				(struct _AT91PS_SvcDataFlash *, u_char, u_int, u_char *, int);
	AT91S_SvcDataFlashStatus (*MainMemoryToBufferTransfert) (struct _AT91PS_SvcDataFlash *, u_char,u_int);
	
	AT91S_SvcDataFlashStatus (*PagePgmBuf)  				(struct _AT91PS_SvcDataFlash *, u_char, u_char *, u_int, u_int);
	AT91S_SvcDataFlashStatus (*WriteBuffer)  				(struct _AT91PS_SvcDataFlash *, u_char, u_char *, u_int, int);
	AT91S_SvcDataFlashStatus (*WriteBufferToMain)  			(struct _AT91PS_SvcDataFlash *, u_char, u_int);
	
	AT91S_SvcDataFlashStatus (*PageErase)  					(struct _AT91PS_SvcDataFlash *, u_int);
	AT91S_SvcDataFlashStatus (*BlockErase)  				(struct _AT91PS_SvcDataFlash *, u_int);
	
	AT91S_SvcDataFlashStatus (*MainMemoryToBufferCompare) 	(struct _AT91PS_SvcDataFlash *, u_char, u_int);
		
	AT91S_DataflashDesc		 DataFlashDesc;					// dataflash descriptor
	AT91PS_Dataflash		 pDevice;						// Pointer on a dataflash features array  
	char          			 *pData;						// private data

}AT91S_SvcDataFlash, *AT91PS_SvcDataFlash;
/************************************************************************************************************************/
typedef AT91PS_SvcDataFlash (*AT91PF_OpenSvcDataFlash)  						(const AT91PS_APMC, AT91PS_SvcDataFlash);
typedef void 				(*AT91PF_SvcDataFlashHandler) 						(AT91PS_SvcDataFlash , u_int);
typedef AT91PS_SvcDataFlash	(*AT91PF_SvcDataFlashGetStatus) 					(AT91PS_DataflashDesc);
typedef void 				(*AT91PF_SvcDataFlashAbortCommand) 					(AT91PS_DataflashDesc);

typedef AT91PS_SvcDataFlash (*AT91PF_SvcDataFlashPageRead)      				(AT91PS_SvcDataFlash,u_int, u_char *, int);
typedef AT91PS_SvcDataFlash (*AT91PF_SvcDataFlashContinuousRead)      			(AT91PS_SvcDataFlash,int, u_char *, int);
typedef AT91PS_SvcDataFlash (*AT91PF_SvcDataFlashReadBuffer)      				(AT91PS_SvcDataFlash,u_char,u_int, u_char *, int);
typedef AT91PS_SvcDataFlash (*AT91PF_SvcDataFlashMainMemoryToBufferTransfert)   (AT91PS_SvcDataFlash,u_char, u_int);
typedef AT91PS_SvcDataFlash (*AT91PF_SvcDataFlashPagePgmBuf)     				(AT91PS_SvcDataFlash,u_char, u_char *, u_int, u_int);
typedef AT91PS_SvcDataFlash (*AT91PF_SvcDataFlashWriteBuffer)     				(AT91PS_SvcDataFlash,u_char, u_char *, u_int, int);
typedef AT91PS_SvcDataFlash (*AT91PF_SvcDataFlashWriteBufferToMain)     		(AT91PS_SvcDataFlash,u_char, u_int);
typedef AT91PS_SvcDataFlash (*AT91PF_SvcDataFlashPageErase) 					(AT91PS_SvcDataFlash, u_int);
typedef AT91PS_SvcDataFlash (*AT91PF_SvcDataFlashBlockErase) 					(AT91PS_SvcDataFlash, u_int);
typedef AT91PS_SvcDataFlash (*AT91PF_SvcDataFlashMainMemoryToBufferCompare) 	(AT91PS_SvcDataFlash, u_char, u_int);
/************************************************************************************************************************/
extern AT91PS_SvcDataFlash 		AT91F_OpenSvcDataFlash 				(const AT91PS_APMC, AT91PS_SvcDataFlash);
extern void 					AT91F_DataFlashHandler				(AT91PS_SvcDataFlash , u_int);
extern AT91S_SvcDataFlashStatus	AT91F_DataFlashGetStatus			(AT91PS_DataflashDesc);
extern void 					AT91F_DataFlashAbortCommand			(AT91PS_DataflashDesc);

extern AT91S_SvcDataFlashStatus AT91F_DataFlashPageRead 			(AT91PS_SvcDataFlash,u_int,u_char *,int);
extern AT91S_SvcDataFlashStatus AT91F_DataFlashContinuousRead 		(AT91PS_SvcDataFlash ,int ,u_char *,int);
extern AT91S_SvcDataFlashStatus AT91F_DataFlashReadBuffer 			(AT91PS_SvcDataFlash,	u_char,	u_int, u_char *, int);
extern AT91S_SvcDataFlashStatus AT91F_MainMemoryToBufferTransfert	(AT91PS_SvcDataFlash ,u_char ,u_int );
extern AT91S_SvcDataFlashStatus AT91F_DataFlashPagePgmBuf			(AT91PS_SvcDataFlash,u_char,u_char*,u_int,u_int);
extern AT91S_SvcDataFlashStatus AT91F_DataFlashWriteBuffer 			(AT91PS_SvcDataFlash, u_char, u_char *, u_int, int);
extern AT91S_SvcDataFlashStatus AT91F_WriteBufferToMain 			(AT91PS_SvcDataFlash, u_char, u_int);
extern AT91S_SvcDataFlashStatus AT91F_PageErase 					(AT91PS_SvcDataFlash, u_int);
extern AT91S_SvcDataFlashStatus AT91F_BlockErase 					(AT91PS_SvcDataFlash, u_int);
extern AT91S_SvcDataFlashStatus AT91F_MainMemoryToBufferCompare		(AT91PS_SvcDataFlash, u_char, u_int);
/************************************************************************************************************************/

#endif