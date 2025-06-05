/*----------------------------------------------------------------------------	*/
/*      ATMEL Microcontroller Software Support  -  ROUSSET  -			*/
/*----------------------------------------------------------------------------	*/
/* The software is delivered "AS IS" without warranty or condition of any	*/
/* kind, either express, implied or statutory. This includes without		*/
/* limitation any warranty or condition with respect to merchantability or	*/
/* fitness for any particular purpose, or against the infringements of		*/
/* intellectual property rights of others.					*/
/*----------------------------------------------------------------------------	*/
/* File Name           : main.h							*/
/* Object              : Test for new boot validation				*/
/*										*/
/* 1.0 27/03/03 HIi    : Creation						*/
/*----------------------------------------------------------------------------	*/

#ifndef _MAIN_H
#define _MAIN_H

#include "AT91RM9200.h"
#include "stdio.h"
#include "embedded_services.h"

/* AIC definitions */
#define AT91C_AIC_BRANCH_OPCODE ((void (*) ()) 0xE51FFF20) /* ldr, pc, [pc, #-&F20] */

/* Standard Asynchronous Mode : 8 bits , 1 stop , no parity */
#define AT91C_US_ASYNC_MODE ( AT91C_US_USMODE_NORMAL + \
                        AT91C_US_NBSTOP_1_BIT + \
                        AT91C_US_PAR_NONE + \
                        AT91C_US_CHRL_8_BITS + \
                        AT91C_US_CLKS_CLOCK )

#define AT91C_PLLA_VALUE	     	0x2026BE04	/* for 18,432Mhz */ /* 0x20263E04 */
#define DELAY_MAIN_FREQ			1000

#define AT91C_UBOOT_BASE_ADDRESS	0x21f00000
#define AT91C_UBOOT_MAXSIZE	        0x20000
#define CLRSCREEN	                "\033[2J"
#define	AT91C_PAUSE			100;

/* prototypes */
extern void AT91F_DBGU_Printk(char *);
extern void AT91F_ST_ASM_HANDLER(void);
extern void Jump(unsigned int addr);
extern void AT91F_SetPLL(void);
extern unsigned int GetTickCount(void);

#endif
