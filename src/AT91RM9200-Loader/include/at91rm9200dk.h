#ifndef __ASSEMBLER__
typedef unsigned long u_32;
typedef unsigned short u_16;
typedef unsigned char u_8;
/* ARM asynchronous clock */
#define ARM_CLK		((u_32)(60000000))
#else
#define ARM_CLK 60000000
#endif

/*
	ARM Core Mode and Status Bits
*/

#define ARM_MODE_USER           0x10
#define ARM_MODE_FIQ            0x11
#define ARM_MODE_IRQ            0x12
#define ARM_MODE_SVC            0x13
#define ARM_MODE_ABORT          0x17
#define ARM_MODE_UNDEF          0x1B
#define ARM_MODE_SYS            0x1F
#define ARM_MODE_MASK           0x1F

#define I_BIT                   0x80
#define F_BIT                   0x40
#define T_BIT                   0x20

#define IRQ_STACK_SIZE		0x10
#define FIQ_STACK_SIZE		0x04
#define ABT_STACK_SIZE		0x04
#define UND_STACK_SIZE		0x04
#define SVC_STACK_SIZE		0x10
#define USER_STACK_SIZE		0x400

/*
	Peripherals
*/

#define AT91C_BASE_AIC		0xFFFFF000
#define AIC_IVR			0x100
#define AIC_EOICR		0x130
