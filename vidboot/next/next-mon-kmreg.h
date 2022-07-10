#ifndef __NEXT_MON_KMREG_H__

/* format of data from keyboard and mouse */
/* taken from nextdev/kmreg.h */
struct keyboard {
	unsigned int : 16,
		valid: 1,
		alt_right: 1,
		alt_left: 1,
		command_right: 1,
		command_left: 1,
		shift_right: 1,
		shift_left: 1,
		control: 1,
		up_down: 1,
		#define KM_DOWN	0
		#define KM_UP	1
		keycode: 7;
};

struct mouse {
	unsigned int : 16,
		delta_y: 7,
		button_right: 1,
		delta_x: 7,
		button_left: 1;
};

union kybd_event {
	int data;
	struct keyboard k;
};

union mouse_event {
	int data;
	struct mouse m;
};


struct km_mon {
	union kybd_event kybd_event;
	union kybd_event autorepeat_event;
	short x,y;
	short nc_tm, nc_lm, nc_w, nc_h;
	int store;
	int fg, bg;
	short ansi;
	short *cp;
	#define KM_NP	3
	short p[KM_NP];
	volatile short flags; /* couple defines more */
};

/*
 * Data layout for console device.
 */
struct km_console_info
{
	int	pixels_per_word;	/* Pixels per 32 bit word: 16, 4, 2, or 1 */
	int	bytes_per_scanline;
	int	dspy_w;			/* Visible display width in pixels */
	int	dspy_max_w;		/* Display width in pixels */
	int	dspy_h;			/* Visible display height in pixels */
#define KM_CON_ON_NEXTBUS	1	/* flag_bits: Console is NextBus device */
	int	flag_bits;		/* Vendor and NeXT flags */
	int	color[4];		/* Bit pattern for white thru black */
#define KM_HIGH_SLOT	6		/* highest possible console slot. */
	char	slot_num;		/* Slot of console device */
	char	fb_num;			/* Logical frame buffer in slot for console */
	char	byte_lane_id;		/* A value of 1, 4, or 8 */
	int	start_access_pfunc;	/* P-code run before each FB access */
	int	end_access_pfunc;	/* P-code run after each FB access */
	struct	{		/* Frame buffer related addresses to be mapped */
			int	phys_addr;
			int	virt_addr;
			int	size;
#define KM_CON_MAP_ENTRIES	6
#define KM_CON_PCODE		0
#define KM_CON_FRAMEBUFFER	1
#define KM_CON_BACKINGSTORE	2
		} map_addr[KM_CON_MAP_ENTRIES];
	int	access_stack;
};


#define __NEXT_MON_KMREG_H__
#endif