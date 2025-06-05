#------------------------------------------------------------------------------
#-         ATMEL Microcontroller Software Support  -  ROUSSET  -
#------------------------------------------------------------------------------
# The software is delivered "AS IS" without warranty or condition of any
# kind, either express, implied or statutory. This includes without
# limitation any warranty or condition with respect to merchantability or
# fitness for any particular purpose, or against the infringements of
# intellectual property rights of others.
#-----------------------------------------------------------------------------
#- File source          : it_handler.s
#- Object               : Example of IT handler calling a C function
#- Compilation flag     : None
#-
#- 1.0 16/03/01 	ODi : Creation
#------------------------------------------------------------------------------

#include "at91rm9200dk.h"

			.text
			.align
			.global AT91F_ST_ASM_HANDLER, _AT91F_ST_ASM_HANDLER

#------------------------------------------------------------------------------
#- IRQ Entry 
#------------------------------------------------------------------------------
AT91F_ST_ASM_HANDLER:

#- Adjust and save LR_irq in IRQ stack
 	sub         r14, r14, #4
	stmfd       sp!, {r14}

#- Write in the IVR to support Protect Mode
#- No effect in Normal Mode
#- De-assert the NIRQ and clear the source in Protect Mode
	ldr         r14, =AT91C_BASE_AIC
	str         r14, [r14, #AIC_IVR]

#- Save SPSR and r0 in IRQ stack
	mrs         r14, SPSR
	stmfd       sp!, {r0, r14}

#- Enable Interrupt and Switch in SYS Mode
	mrs         r0, CPSR
	bic         r0, r0, #I_BIT
	orr         r0, r0, #ARM_MODE_SYS
	msr         CPSR_c, r0

#- Save scratch/used registers and LR in User Stack
	stmfd       sp!, { r1-r3, r12, r14}

#------------------------------------------------------------------------------
#- Branch to C IT Handler
#------------------------------------------------------------------------------

	ldr     r1, =AT91F_ST_HANDLER
	mov     r14, pc
	bx      r1

#------------------------------------------------------------------------------
#- IRQ Exit
#------------------------------------------------------------------------------

#- Restore scratch/used registers and LR from User Stack
	ldmia       sp!, { r1-r3, r12, r14}

#- Disable Interrupt and switch back in IRQ mode
	mrs         r0, CPSR
	bic         r0, r0, #ARM_MODE_SYS
	orr         r0, r0, #(I_BIT | ARM_MODE_IRQ)
	msr         CPSR_c, r0

#- Mark the End of Interrupt on the AIC
	ldr         r0, =AT91C_BASE_AIC
	str         r0, [r0, #AIC_EOICR]

#- Restore SPSR_irq and r0 from IRQ stack
	ldmia       sp!, {r0, r14}
	msr         SPSR_cxsf, r14

#- Restore adjusted  LR_irq from IRQ stack directly in the PC
	ldmia       sp!, {pc}^
