#include <at91rm9200dk.h>

		.text
		.align

#define AT91_USER_Stack_Begin	0x00204000
#define AT91_Stack_Begin      	0x00204000

/* ------------------------ List of Exported resources -------------------------*/
		.globl	__entry,_entry
		.globl	__stext,_stext
		.globl _start,start
		
/*------------------------------------------------------------------------------*/
/*  Define the vector table.*/
/*  The reset vector jumps to the handler code.*/
/*  All others just dead loop on themselves!*/
/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/*    InitReset. This code is executed with the FLASH at address 0.*/
/*------------------------------------------------------------------------------*/
startoftext:
rstvec:
                B           InitReset 
undefvec:
                B           undefvec 
swivec:
                B           swivec   
pabtvec:
                B           pabtvec  
dabtvec:
                B           dabtvec  
rsvdvec:
                B           rsvdvec  
irqvec:
                ldr         pc, [pc,#-0xF20]        
fiqvec:
                ldr         pc, [pc,#-0xF20]       

#-------------------
#- The reset handler
#-------------------
InitReset:

#------------------------------------------------------------------------------
#-Low level Init (PMC, AIC, EBI, ....)
#------------------------------------------------------------------------------
	
	ldr 	r1, = AT91_USER_Stack_Begin
# Insure word alignement
	bic     r1, r1, #3                  
# Init stack USER
	mov     sp, r1                      
	
	bl 	AT91F_LowLevelInit
    
#---------------------------------
#- Setup the stack for each mode
#---------------------------------
   ldr		r0, =AT91_Stack_Begin

#- Set up Supervisor Mode and set SVC Mode Stack
	msr     CPSR_c, #(ARM_MODE_SVC | I_BIT | F_BIT)
# Init stack FIQ
	mov     r13, r0                     
 	sub     r0, r0, #SVC_STACK_SIZE

#- Set up Interrupt Mode and set IRQ Mode Stack
	msr     CPSR_c, #(ARM_MODE_IRQ | I_BIT | F_BIT)
# Init stack IRQ
	mov     r13, r0                     
	sub     r0, r0, #IRQ_STACK_SIZE
 
#- Set up Fast Interrupt Mode and set FIQ Mode Stack
	msr     CPSR_c, #(ARM_MODE_FIQ | I_BIT | F_BIT)
# Init stack Abort
	mov     r13, r0
	sub     r0, r0, #FIQ_STACK_SIZE

#- Set up Abort Mode and set Abort Mode Stack
	msr     CPSR_c, #(ARM_MODE_ABORT | I_BIT | F_BIT)
# Init stack Abort
	mov     r13, r0
	sub     r0, r0, #ABT_STACK_SIZE
	
#- Set up Undefined Instruction Mode and set Undef Mode Stack
	msr     CPSR_c, #(ARM_MODE_UNDEF | I_BIT | F_BIT)
# Init stack Undef
	mov     r13, r0 
	sub     r0, r0, #UND_STACK_SIZE

#- Set up User Mode, set User Mode Stack and enable interrupts  
	msr     CPSR_c, #(ARM_MODE_SYS | F_BIT)
# Init stack Sup
	mov     r13, r0


#------------------------------------------------------------------------------
#- Branch on C code Main function (with interworking)
#----------------------------------------------------
#- Branch must be performed by an interworking call as either an ARM or Thumb 
#- main C function must be supported. This makes the code not position-
#- independant. A Branch with link would generate errors 
#------------------------------------------------------------------------------
 		b _main


#------------------------------------------------------------------------------
#- Loop for ever
#---------------
#- End of application. Normally, never occur.
#- Could jump on Software Reset ( B 0x0 ).
#------------------------------------------------------------------------------
End:
	b           End
