//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : AT91S_SVC_CRC.h
//* Object              : CRC service
//* Creation            : Hi   10/14/2002
//*
//*----------------------------------------------------------------------------
#ifndef AT91S_SVC_CRC_H
#define AT91S_SVC_CRC_H

/////////////////////////////////////////////////////////////////////////////////////////////
extern const unsigned char bit_rev[256];

extern void CalculateCrc32(const unsigned char *,unsigned int, unsigned int *);
extern void CalculateCrc16(const unsigned char *, unsigned int , unsigned short *); 
extern void CalculateCrcHdlc(const unsigned char *, unsigned int, unsigned short *);
extern void CalculateCrc16ccitt(const unsigned char *, unsigned int , unsigned short *);
/////////////////////////////////////////////////////////////////////////////////////////////
typedef const unsigned char* AT91PS_SVC_CRC_BIT_REV ;

typedef void  (*AT91PF_SVC_CRC32)   (const unsigned char *, unsigned int, unsigned int *);
typedef void  (*AT91PF_SVC_CRC16)   (const unsigned char *, unsigned int, unsigned short *);
typedef void  (*AT91PF_SVC_CRCHDLC) (const unsigned char *, unsigned int, unsigned short *);
typedef	void  (*AT91PF_SVC_CRCCCITT)(const unsigned char *, unsigned int , unsigned short *);
/////////////////////////////////////////////////////////////////////////////////////////////


#endif
