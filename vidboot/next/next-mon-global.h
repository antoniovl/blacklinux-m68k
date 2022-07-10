#ifndef __NEXT_MON_GLOBAL_H__

#include "types.h"

#include "next/next-mon-nvram.h"
#include "next/next-mon-sio.h"
#include "next/next-mon-kmreg.h"
#include "next/next-mon-region.h"
#include "next/next-mon-animate.h"

#define N_SIMM	4
#define LMAX	128
#define NBOOTFILE	64
#define NADDR	8

struct mon_global {
	char mg_simm[N_SIMM];	/* MUST BE FIRST (accesed early by locore) */
	char mg_flags;		/* MUST BE SECOND */
#define	MGF_LOGINWINDOW		0x80
#define	MGF_UART_SETUP		0x40
#define	MGF_UART_STOP		0x20
#define	MGF_UART_TYPE_AHEAD	0x10
#define	MGF_ANIM_RUN		0x08
#define	MGF_SCSI_INTR		0x04
#define	MGF_KM_EVENT		0x02
#define	MGF_KM_TYPE_AHEAD	0x01
	unsigned int mg_sid, mg_pagesize, mg_mon_stack, mg_vbr;
	struct nvram_info mg_nvram;
	char mg_inetntoa[18];
	char mg_inputline[LMAX];
	struct mon_region mg_region[N_SIMM];
	caddr_t mg_alloc_base, mg_alloc_brk;
	char *mg_boot_dev, *mg_boot_arg, *mg_boot_info, *mg_boot_file;
	char mg_bootfile[NBOOTFILE];
	enum SIO_ARGS mg_boot_how;
	struct km_mon km;
	int mon_init;
	struct sio *mg_si;
	int mg_time;
	char *mg_sddp;
	char *mg_dgp;
	char *mg_s5cp;
	char *mg_odc, *mg_odd;
	char mg_radix;
	int mg_dmachip;
	int mg_diskchip;
	volatile int *mg_intrstat;
	volatile int *mg_intrmask;
	void (*mg_nofault)();
	char fmt;
	int addr[NADDR], na;
	int	mx, my;			/* mouse location */
	unsigned int	cursor_save[2][32];
	int (*mg_getc)(), (*mg_try_getc)(), (*mg_putc)();
	int (*mg_alert)(), (*mg_alert_confirm)();
	caddr_t (*mg_alloc)();
	int (*mg_boot_slider)();
	volatile unsigned char *eventc_latch;
	volatile unsigned int event_high;
	struct animation *mg_animate;
	int mg_anim_time;
	void (*mg_scsi_intr)();
	int mg_scsi_intrarg;
	short mg_minor, mg_seq;
	int (*mg_anim_run)();
	short mg_major;
	char *mg_clientetheraddr;
	int mg_console_i;
	int mg_console_o;
#define	CONS_I_KBD	0
#define	CONS_I_SCC_A	1
#define	CONS_I_SCC_B	2
#define	CONS_I_NET	3
#define	CONS_O_BITMAP	0
#define	CONS_O_SCC_A	1
#define	CONS_O_SCC_B	2
#define	CONS_O_NET	3
	char *test_msg;
	/* Next entry should be km_coni. Mach depends on this! */
	struct km_console_info km_coni;	/* Console configuration info. See kmreg.h */
	char *mg_fdgp;
	char mg_machine_type, mg_board_rev;
	int (*mg_as_tune)();
	int mg_flags2;
#define	MGF2_PARITY	0x80000000
};


#define __NEXT_MON_GLOBAL_H__
#endif