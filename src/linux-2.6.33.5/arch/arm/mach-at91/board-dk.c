/*
 * linux/arch/arm/mach-at91/board-dk.c
 *
 *  Copyright (C) 2005 SAN People
 *
 *  Epson S1D framebuffer glue code is:
 *     Copyright (C) 2005 Thibaut VARENE <varenet@parisc-linux.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/types.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/mtd/physmap.h>
#include <linux/can/platform/sja1000.h>
#include <linux/interrupt.h>
#include <linux/clk.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/irq.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/hardware.h>
#include <mach/board.h>
#include <mach/gpio.h>
#include <mach/at91rm9200_mc.h>
#include <mach/at91_pio.h>

#include "generic.h"


static void __init dk_map_io(void)
{
	/* Initialize processor: 18.432 MHz crystal */
	at91rm9200_initialize(18432000, AT91RM9200_BGA);

	/* Setup the LEDs */
	at91_init_leds(AT91_PIN_PB2, AT91_PIN_PB2);

	/* DBGU on ttyS0. (Rx & Tx only) */
	at91_register_uart(0, 0, 0);

	/* USART1 on ttyS1. (Rx, Tx, CTS, RTS, DTR, DSR, DCD, RI) */
	at91_register_uart(AT91RM9200_ID_US1, 1, ATMEL_UART_CTS | ATMEL_UART_RTS
			   | ATMEL_UART_DTR | ATMEL_UART_DSR | ATMEL_UART_DCD
			   | ATMEL_UART_RI);

	/* set serial console to ttyS0 (ie, DBGU) */
	at91_set_serial_console(0);
}

static void __init dk_init_irq(void)
{
	at91rm9200_init_interrupts(NULL);
}

static struct at91_eth_data __initdata dk_eth_data = {
	.phy_irq_pin	= AT91_PIN_PC4,
	.is_rmii	= 1,
};

static struct at91_usbh_data __initdata dk_usbh_data = {
	.ports		= 2,
};

static struct at91_udc_data __initdata dk_udc_data = {
	.vbus_pin	= AT91_PIN_PD4,
	.pullup_pin	= AT91_PIN_PD5,
};

static struct at91_cf_data __initdata dk_cf_data = {
	.det_pin	= AT91_PIN_PB0,
	.rst_pin	= AT91_PIN_PC5,
	// .irq_pin	= ... not connected
	// .vcc_pin	= ... always powered
};

static struct at91_mmc_data __initdata dk_mmc_data = {
	.slot_b		= 0,
	.wire4		= 1,
};

static struct spi_board_info dk_spi_devices[] = {
	{	/* DataFlash chip */
		.modalias	= "mtd_dataflash",
		.chip_select	= 0,
		.max_speed_hz	= 15 * 1000 * 1000,
	},
	{	/* UR6HCPS2-SP40 PS2-to-SPI adapter */
		.modalias	= "ur6hcps2",
		.chip_select	= 1,
		.max_speed_hz	= 250 *  1000,
	},
	{	/* TLV1504 ADC, 4 channels, 10 bits; one is a temp sensor */
		.modalias	= "tlv1504",
		.chip_select	= 2,
		.max_speed_hz	= 20 * 1000 * 1000,
	},
#ifdef CONFIG_MTD_AT91_DATAFLASH_CARD
	{	/* DataFlash card */
		.modalias	= "mtd_dataflash",
		.chip_select	= 3,
		.max_speed_hz	= 15 * 1000 * 1000,
	}
#endif
};

static struct i2c_board_info __initdata dk_i2c_devices[] = {
	{
		I2C_BOARD_INFO("ics1523", 0x26),
	},
	{
		I2C_BOARD_INFO("x9429", 0x28),
	},
	{
		I2C_BOARD_INFO("24c1024", 0x50),
	}
};

static struct mtd_partition __initdata dk_nand_partition[] = {
	{
		.name	= "NAND Partition 1",
		.offset	= 0,
		.size	= MTDPART_SIZ_FULL,
	},
};

static struct mtd_partition * __init nand_partitions(int size, int *num_partitions)
{
	*num_partitions = ARRAY_SIZE(dk_nand_partition);
	return dk_nand_partition;
}

static struct atmel_nand_data __initdata dk_nand_data = {
	.ale		= 22,
	.cle		= 21,
	.det_pin	= AT91_PIN_PB1,
	.rdy_pin	= AT91_PIN_PC2,
	// .enable_pin	= ... not there
	.partition_info	= nand_partitions,
};

#define DK_FLASH_BASE	AT91_CHIPSELECT_0
#define DK_FLASH_SIZE	SZ_16M

static struct mtd_partition nor_flash_partitions[] = {
    {
        .name   = "bootloader",      // 分区0: boot.bin + u-boot.bin.gz
        .offset = 0x00000000,    // 起始地址
        .size   = 0x00040000,    // 大小256KB
		.mask_flags = MTD_WRITEABLE, // 可选：设为只读
    }, {
        .name   = "kernel",      // 分区1: 内核
        .offset = 0x00040000,    // 起始地址
        .size   = 0x00200000,    // 大小2MB
		.mask_flags = MTD_WRITEABLE, // 可选：设为只读
    }, {
        .name   = "rootfs",      // 分区2: 根文件系统（MTD2）
        .offset = 0x00240000,
        .size   = 0x00DC0000,    // （JS28F128J3F75A剩余总容量）
    },
};

static struct physmap_flash_data dk_flash_data = {
	.width		= 2,
	.nr_parts	= ARRAY_SIZE(nor_flash_partitions),
	.parts		= nor_flash_partitions,
};

static struct resource dk_flash_resource = {
	.start		= DK_FLASH_BASE,
	.end		= DK_FLASH_BASE + DK_FLASH_SIZE - 1,
	.flags		= IORESOURCE_MEM,
};

static struct platform_device dk_flash = {
	.name		= "physmap-flash",
	.id		= 0,
	.dev		= {
				.platform_data	= &dk_flash_data,
			},
	.resource	= &dk_flash_resource,
	.num_resources	= 1,
};

static struct gpio_led dk_leds[] = {
	{
		.name			= "led0",
		.gpio			= AT91_PIN_PB2,
		.active_low		= 1,
		.default_trigger	= "heartbeat",
	}
};

#define DK_CAN_ADDRESS_SPACE_SIZE	128

static struct sja1000_platform_data dk_can_data = {
	.clock		= 8000000,
	.ocr		= 0xfa,
	.cdr		= 0x88,
};

static struct resource dk_can0_resource[] = {
	[0] = {
		.start		= AT91_CHIPSELECT_3,
		.end		= AT91_CHIPSELECT_3 + DK_CAN_ADDRESS_SPACE_SIZE - 1,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91RM9200_ID_IRQ2,
		.end	= AT91RM9200_ID_IRQ2,
		.flags	= IORESOURCE_IRQ | IRQF_TRIGGER_HIGH,
	},
};

static struct resource dk_can1_resource[] = {
	[0] = {
		.start		= AT91_CHIPSELECT_4,
		.end		= AT91_CHIPSELECT_4 + DK_CAN_ADDRESS_SPACE_SIZE - 1,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91RM9200_ID_IRQ3,
		.end	= AT91RM9200_ID_IRQ3,
		.flags	= IORESOURCE_IRQ | IRQF_TRIGGER_HIGH,
	},
};

static struct resource dk_can2_resource[] = {
	[0] = {
		.start		= AT91_CHIPSELECT_6,
		.end		= AT91_CHIPSELECT_6 + DK_CAN_ADDRESS_SPACE_SIZE - 1,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91RM9200_ID_IRQ4,
		.end	= AT91RM9200_ID_IRQ4,
		.flags	= IORESOURCE_IRQ | IRQF_TRIGGER_HIGH,
	},
};

static struct resource dk_can3_resource[] = {
	[0] = {
		.start		= AT91_CHIPSELECT_7,
		.end		= AT91_CHIPSELECT_7 + DK_CAN_ADDRESS_SPACE_SIZE - 1,
		.flags		= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91RM9200_ID_IRQ5,
		.end	= AT91RM9200_ID_IRQ5,
		.flags	= IORESOURCE_IRQ | IRQF_TRIGGER_HIGH,
	},
};

static struct platform_device dk_can0 = {
	.name		= "sja1000_platform",
	.id		= 0,
	.dev		= {
				.platform_data	= &dk_can_data,
			},
	.resource	= dk_can0_resource,
	.num_resources	= ARRAY_SIZE(dk_can0_resource),
};

static struct platform_device dk_can1 = {
	.name		= "sja1000_platform",
	.id		= 1,
	.dev		= {
				.platform_data	= &dk_can_data,
			},
	.resource	= dk_can1_resource,
	.num_resources	= ARRAY_SIZE(dk_can1_resource),
};

static struct platform_device dk_can2 = {
	.name		= "sja1000_platform",
	.id		= 2,
	.dev		= {
				.platform_data	= &dk_can_data,
			},
	.resource	= dk_can2_resource,
	.num_resources	= ARRAY_SIZE(dk_can2_resource),
};

static struct platform_device dk_can3 = {
	.name		= "sja1000_platform",
	.id		= 3,
	.dev		= {
				.platform_data	= &dk_can_data,
			},
	.resource	= dk_can3_resource,
	.num_resources	= ARRAY_SIZE(dk_can3_resource),
};

void __init at91_add_device_cans(struct sja1000_platform_data *data)
{
	unsigned int csa;

	if (!data)
		return;

	/* enable the address range of CS3/CS4*/
	// SJA1000_1和SJA1000_2对应与CS3和CS4
	// SJA1000_3和SJA1000_4对应的CS是不可编程的，默认的，无需设置
	csa = at91_sys_read(AT91_EBI_CSA);
	csa &= ~AT91_EBI_CS3A;
	csa |= AT91_EBI_CS3A_SMC;
	csa &= ~AT91_EBI_CS4A;
	csa |= AT91_EBI_CS4A_SMC;
	at91_sys_write(AT91_EBI_CSA, csa);

	/* set the bus interface characteristics */
	// SJA1000_1
	//at91_sys_write(AT91_SMC_CSR(3), (0x2 | (0x1 << 7) | (0x2 << 8) | (0x2 << 13) | (0x0 << 15)));
	at91_sys_write(AT91_SMC_CSR(3), (AT91_SMC_NWS_(0x2) | AT91_SMC_WSEN 
		| AT91_SMC_TDF_(0x2) | AT91_SMC_DBW_8 | (0x0 << 15)));

	// SJA1000_2
    at91_sys_write(AT91_SMC_CSR(4), (AT91_SMC_NWS_(0x2) | AT91_SMC_WSEN 
		| AT91_SMC_TDF_(0x2) | AT91_SMC_DBW_8 | (0x0 << 15)));
	
	// SJA1000_3
    at91_sys_write(AT91_SMC_CSR(6), (AT91_SMC_NWS_(0x2) | AT91_SMC_WSEN 
		| AT91_SMC_TDF_(0x2) | AT91_SMC_DBW_8 | (0x0 << 15)));

	// SJA1000_4
	at91_sys_write(AT91_SMC_CSR(7), (AT91_SMC_NWS_(0x2) | AT91_SMC_WSEN 
		| AT91_SMC_TDF_(0x2) | AT91_SMC_DBW_8 | (0x0 << 15)));

	//中断线
	at91_set_B_periph(AT91_PIN_PA2, 0);
	at91_set_B_periph(AT91_PIN_PA3, 0);
	at91_set_B_periph(AT91_PIN_PA23, 0);
	at91_set_B_periph(AT91_PIN_PA25, 0);

	platform_device_register(&dk_can0);
	platform_device_register(&dk_can1);
	platform_device_register(&dk_can2);
	platform_device_register(&dk_can3);
}

static void __init dk_board_init(void)
{
	/* Serial */
	at91_add_device_serial();
	/* Ethernet */
	at91_add_device_eth(&dk_eth_data);
	/* USB Host */
	at91_add_device_usbh(&dk_usbh_data);
	/* USB Device */
	at91_add_device_udc(&dk_udc_data);
	at91_set_multi_drive(dk_udc_data.pullup_pin, 1);	/* pullup_pin is connected to reset */
	/* Compact Flash */
	at91_add_device_cf(&dk_cf_data);
	/* I2C */
	at91_add_device_i2c(dk_i2c_devices, ARRAY_SIZE(dk_i2c_devices));
	/* SPI */
	at91_add_device_spi(dk_spi_devices, ARRAY_SIZE(dk_spi_devices));
#ifdef CONFIG_MTD_AT91_DATAFLASH_CARD
	/* DataFlash card */
	at91_set_gpio_output(AT91_PIN_PB7, 0);
#else
	/* MMC */
	at91_set_gpio_output(AT91_PIN_PB7, 1);	/* this MMC card slot can optionally use SPI signaling (CS3). */
	at91_add_device_mmc(0, &dk_mmc_data);
#endif
	/* NAND */
	at91_add_device_nand(&dk_nand_data);
	/* NOR Flash */
	platform_device_register(&dk_flash);
	/* LEDs */
	at91_gpio_leds(dk_leds, ARRAY_SIZE(dk_leds));
	/* SJA1000 can */
	at91_add_device_cans(&dk_can_data);
	/* VGA */
//	dk_add_device_video();
}

MACHINE_START(AT91RM9200DK, "Atmel AT91RM9200-DK")
	/* Maintainer: SAN People/Atmel */
	.phys_io	= AT91_BASE_SYS,
	.io_pg_offst	= (AT91_VA_BASE_SYS >> 18) & 0xfffc,
	.boot_params	= AT91_SDRAM_BASE + 0x100,
	.timer		= &at91rm9200_timer,
	.map_io		= dk_map_io,
	.init_irq	= dk_init_irq,
	.init_machine	= dk_board_init,
MACHINE_END
