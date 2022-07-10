#ifndef __NEXT_NVRAM_H__

struct nvram_info {
	unsigned int
		#define NI_RESET	9
		ni_reset: 4,
		#define SCC_ALT_CONS	0x08000000
		ni_alt_cons: 1,
		#define ALLOW_EJECT		0x04000000
		ni_allow_eject: 1,
		ni_vol_r: 6,
		ni_brightness: 6,
		#define HW_PWD	0x6
		ni_hw_pwd: 4,
		ni_vol_l: 6,
		ni_spkren: 1,
		ni_lowpass: 1,
		#define BOOT_ANY		0x00000002
		ni_boot_any: 1,
		#define ANY_CMD			0x00000001
		ni_any_cmd: 1;
	#define NVRAM_HW_PASSWD	6
	unsigned char ni_ep[NVRAM_HW_PASSWD];
	#define ni_enetaddr	ni_ep
	#define ni_hw_passwd	ni_ep
	unsigned short ni_simm;	/* 4 simms, 4 bits per simm */
	char ni_adobe[2];
	unsigned char ni_pot[3];
	unsigned char 
		ni_new_clock_chip: 1,
		ni_auto_poweron: 1,
		ni_use_console_slot: 1,
		ni_console_slot: 2,
		ni_use_parity_mem: 1,
		:2;
	#define NVRAM_BOOTCMD	12
	char ni_bootcmd[NVRAM_BOOTCMD];
	unsigned short ni_chksum;		
};

/* lacks several #defines */

#define __NEXT_NVRAM_H__
#endif