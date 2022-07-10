/*
 * Global variables and machine config stuff.
 */
#ifndef __MAIN_SETUP_H__

#include "next/next-mon-global.h"
#include "types.h"

/* Declared in console.c */
extern unsigned long BI_videoaddr, /* FB video addr */
    BI_videorow, BI_videoheight,   /* Height of screen in pixels */
    BI_videowidth,                 /* Width of screen in pixels */
    BI_dimensions, BI_nppw,        /* Number of pixel per word */
    BI_nbpp,                       /* Number of bits per pixel */
    BI_colormask, BI_white,        /* White color definition */
    BI_lt_gray,                    /* Light gray definition */
    BI_dk_gray,                    /* Dark gray definition */
    BI_black;                      /* Black color definition */

extern char machine_string[255]; /* Msg displayed at boot time */

struct mon_global *mg; /* pointer to mon_global */

/* this is slot_id+value */
#define MON_ADDR 0x0200e000
#define SCR1_ADDR 0x0200c000

#define M68040 1
#define M68030 0

/*
 * macros to decode scr1
 */
#define scr1_machine_type(x) ((x) >> 4)
#define scr1_board_revision(x) ((x)&0xf)
#define CPU_40MHZ 0
#define CPU_20MHZ 1
#define CPU_25MHZ 2
#define CPU_33MHZ 3
#define MEM_120NS 0
#define MEM_100NS 1
#define MEM_80NS 2
#define MEM_60NS 3
#define NeXT_CUBE 0
#define NeXT_WARP9 1
#define NeXT_X15 2
#define NeXT_WARP9C 3
struct scr1 {
  unsigned int slot_id : 4, : 4, dma_rev : 8, cpu_rev : 8, videomem_speed : 2,
      mainmem_speed : 2, reserved : 2, cpu_clock : 2;
};

/*
 * Structure with configuration of the machine.
 * Used by machine_setup().
 */
#define VIDEO_COLOR 1
#define VIDEO_MONO 0
struct machine_config {
  unsigned slot_id;
  unsigned long slot_id_bmap;
  struct scr1 scr1;
  unsigned char cpu_type; /* M68040, M68030 */
  char machine_string[255];
  char video_type; /* VIDEO_MONO, VIDEO_COLOR */
  int dma_rev,     /* Variables to make easier access */
      cpu_rev, machine_type, board_revision, videomem_speed, mainmem_speed,
      cpu_clock;
};

int machine_setup(int); /* Sets things up depending machine type */

/*****************************
 * Machine Defnitions        *
 *****************************/

/*
 * Non-Turbo Mono Station:
 *	- 1152x864
 *	- 0x0b000000 mono framebuffer
 */
#define NTM_VIDEO 0x0b000000
#define NTM_VIDEOW 1152
#define NTM_VIDEOH 864
#define NTM_NPPW 16
/* This color values should be 0x00, as here we
   are accessing video mem as a pointer to
   a unsigned char. */
#define NTM_WHITE 0x00
#define NTM_LT_GRAY 0x55
#define NTM_DK_GRAY 0xaa
#define NTM_BLACK 0xff
#define NTM_MACHINE_STRING "Non Turbo Mono "
#define MACHINE_NTM 1

/*
 * Non-Turbo Color
 *  - 1120x832 (I guess)
 *  - 0x2c000000 color framebuffer
 */
#define NTC_VIDEO 0x2c000000
#define NTC_VIDEOW 1120
#define NTC_VIDEOH 832
#define NTC_NPPW 2
/* In this way is easier to handle it. This corresponds
   to a plot of a whole word, that is 2 pixels:
   0xffff0000  - pixel 1
   0x0000ffff  - pixel 2
   console_plotpixel() requires this as we're handling
   the above masks to access a long pointer to video mem. */
#define NTC_WHITE 0xfff0fff0
#define NTC_LT_GRAY 0xaaa0aaa0
#define NTC_DK_GRAY 0x55505550
#define NTC_BLACK 0x00000000
#define NTC_MACHINE_STRING "Non Turbo Color "
#define MACHINE_NTC 2

/*
 * Turbo Mono Station:
 *	- 1120x832
 *	- 0x0cfc0000 mono framebuffer
 */
#define TM_VIDEO 0x0cfc0000
#define TM_VIDEOW 1120
#define TM_VIDEOH 832
#define TM_NPPW 16
#define TM_WHITE 0x00
#define TM_LT_GRAY 0x55
#define TM_DK_GRAY 0xaa
#define TM_BLACK 0xff
#define TM_MACHINE_STRING "Turbo Mono "
#define MACHINE_TM 3

/*
 * Turbo Color
 *  - 1120x832 (1152x832?)
 *  - 0x2c000000 color framebuffer
 */
#define TC_VIDEO 0x2c000000
#define TC_VIDEOW 1120
#define TC_VIDEOH 832
#define TC_NPPW 2
#define TC_WHITE 0xfff0fff0
#define TC_LT_GRAY 0xaaa0aaa0
#define TC_DK_GRAY 0x55505550
#define TC_BLACK 0x00000000
#define TC_MACHINE_STRING "Turbo Color "
#define MACHINE_TC 4

/*
 * Cube with NeXTdimension board
 *  - 1152x864 (I guess)
 *  - 32bit per pixel
 *  - 0x2c000000 color framebuffer
 */
#define CDB_VIDEO 0x2c000000
#define CDB_VIDEOW 1152
#define CDB_VIDEOH 864
#define CDB_NPPW 1
#define CDB_WHITE 0xffffffff
#define CDB_LT_GRAY 0xaaaaaaaa
#define CDB_DK_GRAY 0x55555555
#define CDB_BLACK 0x00000000
#define CDB_MACHINE_STRING "Cube with a NeXTdimension Board"
#define MACHINE_CDB 5

#define __MAIN_SETUP_H__
#endif