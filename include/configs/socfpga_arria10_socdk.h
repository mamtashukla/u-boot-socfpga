/* SPDX-License-Identifier: GPL-2.0 */
/*
 *  Copyright (C) 2015-2019 Altera Corporation <www.altera.com>
 */

#ifndef __CONFIG_SOCFGPA_ARRIA10_H__
#define __CONFIG_SOCFGPA_ARRIA10_H__

#include <asm/arch/base_addr_a10.h>
#include "../arch/arm/mach-socfpga/include/mach/clock_manager_arria10.h"
#define CONFIG_SYS_BOOTM_LEN	(32 * 1024 * 1024)


#define CONFIG_SILENT_CONSOLE
#define CONFIG_SYS_DEVICE_NULLDEV
#define CONFIG_SILENT_CONSOLE_UPDATE_ON_RELOC
#define CONFIG_BOARD_EARLY_INIT_F 1

/*
 * U-Boot general configurations
 */

/* Memory configurations  */
#define PHYS_SDRAM_1_SIZE		0x40000000

/*
 * Serial / UART configurations
 */
#define CONFIG_SYS_NS16550_MEM32
#define CONFIG_SYS_BAUDRATE_TABLE {4800, 9600, 19200, 38400, 57600, 115200}
#define CONFIG_SYS_NAND_U_BOOT_OFFS	0x100000

#define CONFIG_SYS_NS16550_COM1		SOCFPGA_UART0_ADDRESS
#define CONFIG_SYS_NS16550_CLK	    cm_get_l4_sp_clk_hz()

/*
 * L4 OSC1 Timer 0
 */
/* reload value when timer count to zero */
#define TIMER_LOAD_VAL			0xFFFFFFFF

/*
 * Flash configurations
 */

/* SPL memory allocation configuration, this is for FAT implementation */

/* these are in devault environment so they must be always defined */

#define CONFIG_SYS_SPL_MALLOC_SIZE	0x00015000

//#define CONFIG_SYS_DTB_ADDR 0x100
#define MAX_DTB_SIZE_IN_RAM 0xff00
#if ((CONFIG_SYS_DTB_ADDR + MAX_DTB_SIZE_IN_RAM) > CONFIG_SYS_LOAD_ADDR)
#error "MAX_DTB_SIZE_IN_RAM is too big. It will overwrite zImage in memory."
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	"verify=y\0" \
	"loadaddr=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"fdtaddr=" __stringify(CONFIG_SYS_DTB_ADDR) "\0" \
	"bootimage=zImage\0" \
	"bootimagesize=0x600000\0" \
	"fdtimagesize=" __stringify(MAX_DTB_SIZE_IN_RAM) "\0" \
	"mmcloadcmd=fatload\0" \
	"mmcloadpart=1\0" \
	"mmcrootpart=3\0" \
	"mmcroot=/dev/mmcblk0p\0" \
	/* Load kernel and device tree from eMMC to RAM. */ \
	"mmcload=mmc rescan &&" \
		"${mmcloadcmd} mmc 0:${mmcloadpart} ${loadaddr} ${bootimage} &&" \
		"${mmcloadcmd} mmc 0:${mmcloadpart} ${fdtaddr} ${fdtfile}\0" \
	/* Main command. */ \
	"bootcmd=" CONFIG_BOOTCOMMAND "\0" \
	"u-boot_swstate_reg=0xffd0620c\0" \
	"u-boot_image_valid=0x49535756\0" \
	"ethaddr=00:14:d1:b0:7b:69\0" \
	"serverip=192.168.2.2\0" \
	"ipaddr=192.168.2.1\0" \
	"gatewayip=192.168.2.2\0" \
	"netmask=255.255.255.0\0" \
	"bs_file=BOOT.STS\0" \
	"bs_file_size=1\0" \
	"tmp_var_0=0x3fff000\0" \
	"tmp_var_1=0x3fff100\0" \
	"tmp_data=0x4000000\0" \
	"mmcloadpart=1\0" \
	"mmcinstpart=3\0" \
	"mmcrunpart=4\0" \
	\
	/* Address of register in FPGA lightweight bridge which holds HW revision */ \
	"fpga_hwrev_reg=0xff200118\0" \
	 /* Enable lightweight FPGA bridge, read HW revision register and check
	  * if it matches content of ${tmp_var_0} variable. */ \
	"chk_hwrev_var0=cmp ${fpga_hwrev_reg} ${tmp_var_0} 1\0" \
	/* Check if we are running on actual KREA HW or an outdated one*/ \
	"chk_hwrev_new_ddr=mw ${tmp_var_0} 4; run chk_hwrev_var0\0" \
	"chk_hwrev_prod=mw ${tmp_var_0} 3; run chk_hwrev_var0\0" \
    "chk_hwrev_p2=mw ${tmp_var_0} 2; run chk_hwrev_var0\0" \
	"check_hw=if run chk_hwrev_p2; then " \
        "echo '[KREA Proto-2 HW Platform]'; " \
        "setenv instimg leica-image-installer.img; " \
        "setenv fdtfile krea_pt2.dtb; " \
	"else; " \
        "if run chk_hwrev_prod; then " \
            "echo '[KREA HW Platform]'; " \
            "setenv instimg leica-image-installer.img; " \
            "setenv fdtfile krea.dtb; " \
        "else " \
			"if run chk_hwrev_new_ddr; then " \
				"echo '[KREA HW Platform with new DDR]'; " \
				"setenv instimg leica-image-installer.img; " \
				"setenv fdtfile krea.dtb; " \
			"else " \
				"echo '[ERROR]: KREA HW has not been identified! Boot process would stop!'; " \
			"fi; " \
		"fi; " \
	"fi\0 " \
	/* Try to get an installer image from TFTP server in a loop. */ \
	"tftp_loop=setenv filesize 0; " \
		"while test \"${filesize}\" = \"0\"; do " \
			"tftp ${tmp_data} ${instimg}; sleep 1; " \
		"done; true \0" \
	/* Get installer image and write it to eMMC. */ \
	"loadfs=run tftp_loop && setexpr blkcnt $filesize + 0x1ff && setexpr blkcnt $blkcnt / 0x200 && " \
	       "mmc write ${tmp_data} 0x0 ${blkcnt}\0" \
	/* Make sure that kernel image and device tree are present on
	 *${mmcloadpart} FAT partition. */ \
	"chk_krn=fatsize mmc 0:${mmcloadpart} ${bootimage} && fatsize mmc 0:${mmcloadpart} ${fdtfile}\0" \
	/* Make sure that file used to control U-Boot behaviour is present
	 * on FAT partition. */ \
	"chk_boot_ex=fatsize mmc 0:${mmcloadpart} ${bs_file}\0" \
	/* Set content of file used to control U-boot behaviour to 0... */ \
	"set_boot_0=mw ${tmp_var_0} 0; run set_boot_var0\0" \
	/* ... or 1 ... */ \
	"set_boot_1=mw ${tmp_var_0} 1; run set_boot_var0\0" \
	/* ... or 2 . */ \
	"set_boot_2=mw ${tmp_var_0} 2; run set_boot_var0\0" \
	/* Macro used to set content of file used to control U-Boot behaviour
	 * to value stored in ${tmp_var_0} variable. */ \
	"set_boot_var0=fatwrite mmc 0:${mmcloadpart} ${tmp_var_0} ${bs_file} ${bs_file_size}\0" \
	/* Check if file used to control U-boot behaviour contains 0 ... */ \
	"chk_boot_0=mw ${tmp_var_0} 0; run chk_boot_var0\0" \
	/* ... or 1 ... */ \
	"chk_boot_1=mw ${tmp_var_0} 1; run chk_boot_var0\0" \
	/* ... or 2. */ \
	"chk_boot_2=mw ${tmp_var_0} 2; run chk_boot_var0\0" \
	/* Macro used to check if content of file used to control U-boot behaviour
	 * matches content of ${tmp_var_0} variable. */ \
	"chk_boot_var0=fatload mmc 0:${mmcloadpart} ${tmp_var_1} ${bs_file} ${bs_file_size} && " \
		"cmp.b ${tmp_var_0} ${tmp_var_1} ${bs_file_size}\0" \
	"startcmd=if run chk_boot_ex && run chk_krn; then " \
		"run chk_boot_status; " \
	"else " \
		"run loadfs; " \
	"fi\0" \
	/* Check value of file used to control U-Boot behaviour and act accordingly. */ \
	"chk_boot_status=if run chk_boot_0; then " \
		"echo 'BOOT STAGE: [0 - Initial Load]'; run loadfs; " \
	"elif run chk_boot_1; then " \
		"echo 'BOOT STAGE: [1 - Updater First Stage]'; run set_boot_0; setenv mmcrootpart ${mmcinstpart}; " \
	"elif run chk_boot_2; then " \
		"echo 'BOOT STAGE: [2 - Updater Second Stage]'; run set_boot_1; setenv mmcrootpart ${mmcrunpart}; " \
	"else " \
		"echo 'BOOT STAGE: [Normal Boot]'; setenv mmcrootpart ${mmcrunpart}; " \
	"fi\0 "




/* The rest of the configuration is shared */
#include <configs/socfpga_common.h>

/*
 * L4 Watchdog
 */
#ifdef CONFIG_HW_WATCHDOG
#undef CONFIG_DW_WDT_BASE
#define CONFIG_DW_WDT_BASE		SOCFPGA_L4WD1_ADDRESS
#endif

#endif	/* __CONFIG_SOCFGPA_ARRIA10_H__ */
