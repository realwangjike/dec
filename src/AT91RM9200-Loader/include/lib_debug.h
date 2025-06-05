//*----------------------------------------------------------------------------
//*      ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : lib_debug.h
//* Object              : Flags definition used to enable debug features
//*
//* 1.0 Sept 3 01 	    : ODi Creation
//*----------------------------------------------------------------------------

 // AT91_DEBUG_ENABLE flag is supposed to be set using a compilation flag.
 // !!!! It must not be used in a release compilation !!!!!
#ifdef AT91_DEBUG_ENABLE

// Debug compilation flag used for AT91S_Pipe object
#define DBG_DRV_PIPE

// Debug compilation flag used for AT91S_Buffer object
#define DBG_DRV_BUFFER

// Debug compilation flag used for AT91S_Periph object
#define DBG_DRV_PERIPH

#endif // DEBUG_ENABLED
