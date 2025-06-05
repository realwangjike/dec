#------------------------------------------------------------------------------
#-      ATMEL Microcontroller Software Support  -   ROUSSET -
#------------------------------------------------------------------------------
#- File source          : remap.s
#- Librarian            : Not applicable
#- Translator           : 
#-
#- Treatment            : Execute the remap and branch to 0
#-
#- 03/10/01   HI 	: Creation
#------------------------------------------------------------------------------
#include <at91rm9200dk.h>

		.text
		.align
		.global Jump, _Jump

Jump:

#- Set up User Mode, set User Mode Stack and disable interrupts  
	msr     CPSR_c, #(ARM_MODE_SYS | I_BIT | F_BIT)
	
	mov pc, r0				
