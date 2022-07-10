/*
 * asm Utilities taken from sdmach.
 * Warning: Not official, nor public -- I'll have to rewrite this.
 */

/* #define DBG */

#include "asm-setup.h"

#define SCR1	0x0200c000
#define ILLEGAL_INSTR	0x10
#define ADDR_ERROR	0xc
#define BUS_ERROR	0x8
#define F_LINE_EMU	0x2c
#define INITIAL_PC	0x4

.globl _start

.globl SYMBOL_NAME(enable_interrupts)
.globl SYMBOL_NAME(disable_interrupts)
.globl SYMBOL_NAME(get_sr)
.globl SYMBOL_NAME(get_cacr)
.globl SYMBOL_NAME(get_vbr)

.globl SYMBOL_NAME(check_cpu)

.globl SYMBOL_NAME(linux_macho_launch)
.globl SYMBOL_NAME(linux_rawbin_launch)
.globl SYMBOL_NAME(get_mon)

/* Debuggin asm display routines */
.globl SYMBOL_NAME(putc)
.globl SYMBOL_NAME(putr)
.globl SYMBOL_NAME(putn)
.globl SYMBOL_NAME(console_putc)	/* needed by putc */

.text

/*
 * Startup Code.
 * This routine it's always _start, even on Linux.
 */
_start:
	/* Register loading */
	movel	r(a0),SYMBOL_NAME(reg_a0)
	movel	r(a1),SYMBOL_NAME(reg_a1)
	movel	r(a2),SYMBOL_NAME(reg_a2)
	movel	r(a3),SYMBOL_NAME(reg_a3)
	movel	r(a4),SYMBOL_NAME(reg_a4)
	movel	r(a5),SYMBOL_NAME(reg_a5)	 
	movel	r(a6),SYMBOL_NAME(reg_a6)
	movel	r(a7),SYMBOL_NAME(reg_a7)
	movel	r(d0),SYMBOL_NAME(reg_d0)
	movel	r(d1),SYMBOL_NAME(reg_d1)
	movel	r(d2),SYMBOL_NAME(reg_d2)
	movel	r(d3),SYMBOL_NAME(reg_d3)
	movel	r(d4),SYMBOL_NAME(reg_d4)
	movel	r(d5),SYMBOL_NAME(reg_d5)
	movel	r(d6),SYMBOL_NAME(reg_d6)
	movel	r(d7),SYMBOL_NAME(reg_d7)

	/* Load *mg (mon_global) structure */
	movec	r(vbr),r(a1)
	movel	r(a1)@(INITIAL_PC),SYMBOL_NAME(mg)
	
	/* Setup parameters to main() */
	movel	r(a0),r(sp)@-
	pea	r(sp)@
	jsr	SYMBOL_NAME(main)
	
	/* This storage space it used as reference to move16. 
	   I need to know a fixed address. Any changes between
	   _start and this will have to be reflected in the
	   move16 opcode in check_cpu. */
	.align	CACHELINE_ALIGNMENT
SYMBOL_NAME(move16_src):
	.long	0xaabbccdd
	.long	0xeeffaabb
	.long	0xccddeeff
	.long	0xaabbccdd
SYMBOL_NAME(move16_dst):
	.long	0x00000000
	.long	0x00000000
	.long	0x00000000
	.long	0x00000000
	.align	2

/*
 * Enables Interrupts
 */
SYMBOL_NAME(enable_interrupts):
	oriw	#0x0700,r(sr)
	rts

/*
 * Disables interrupts. 
 * Registers preserverd.
 */
SYMBOL_NAME(disable_interrupts):
	andiw	#0xf8ff,r(sr)
	rts

/* 
 * Loads the vector base register and returns it in d0
 */
SYMBOL_NAME(get_vbr):
	movec	r(vbr),r(d0)
	rts

/*
 * Load the status register and returns it in d0
 */
SYMBOL_NAME(get_sr):
	movew	r(sr),r(d0)
	rts

/*
 * Loads the cacr
 */
SYMBOL_NAME(get_cacr):
	movec	r(cacr),r(d0)
	rts


/*
 * will try to fetch whatever its at the mon addr
 */
SYMBOL_NAME(get_mon):
	moveml	r(a0)/r(a1),r(sp)@-
	lea	SYMBOL_NAME(slot_id),r(a1)
	movel	r(a1)@,r(a0)
	addal	#0x0200e000,r(a0)
	lea	SYMBOL_NAME(mon_global),r(a1)
	movel	r(a0)@,r(a1)@
	moveml	r(sp)@+,r(a0)/r(a1)
	rts

/*
 * This will load the start address of the kernel image
 * and then jmp to it, skipping 152 bytes of the mach-o
 * header.
 */
SYMBOL_NAME(linux_macho_launch):
	lea	SYMBOL_NAME(linux_start_addr),r(a0)
	movel	r(a0)@,r(a1)	| /* linux_start_addr it's a pointer */
	addal	#152,r(a1)	| /* skip mach-o header */
	jmp	r(a1)@	| /* Let's kick ass ... */
	rts

/*
 * Launches a raw binary image. Doesn't need to skip any headers.
 */
SYMBOL_NAME(linux_rawbin_launch):
	lea	SYMBOL_NAME(linux_start_addr),r(a0)
	movel	r(a0)@,r(a1)	| /* linux_start_addr it's a pointer */
	jmp	r(a1)@	| /* Let's kick ass ... */
	rts


/*
 * Detects cpu type by handling illegal inst. vector.
 * Returns 1 if 040.
 */

SYMBOL_NAME(tmp):
	.long	0x1	| /* 68040 by default */

SYMBOL_NAME(illegal_instr):
#if defined(DBG)
	movel	#0x22,r(sp)@-
	bsr	SYMBOL_NAME(putn)
#endif
	clrl	SYMBOL_NAME(tmp)
	movew	r(sp)@,r(sr)	| /* restore %sr */
	addqw	#8,r(sp)	| /* trash stack frame */
	jmp	SYMBOL_NAME(cont_check_cpu)

SYMBOL_NAME(f_line_emu):
#if defined(DBG)
	movel	#0x23,r(sp)@-
	bsr	SYMBOL_NAME(putn)
#endif
	clrl	SYMBOL_NAME(tmp)
	movew	r(sp)@,r(sr)	| /* restore %sr */
	addqw	#8,r(sp)	| /* trash stack frame */
	jmp	SYMBOL_NAME(cont_check_cpu)


SYMBOL_NAME(check_cpu):
	moveml	r(a0)/r(a1),r(sp)@-

	/* Save & Disable Interrupts */
	movew	r(sr),r(sp)@-
	oriw	#0x0700,r(sr)

	/* Save old bus, addr, illegal and f-line vectors */
	movec	r(vbr),r(a0)
	movel	r(a0)@(ILLEGAL_INSTR),r(sp)@-
	movel	r(a0)@(F_LINE_EMU),r(sp)@-

	/* Load our handlers. */
	movec	r(vbr),r(a0)
	movel	#SYMBOL_NAME(illegal_instr),r(a0)@(ILLEGAL_INSTR)
	movel	#SYMBOL_NAME(f_line_emu),r(a0)@(F_LINE_EMU)

	/* let's try a 68040 only instr */
	lea	SYMBOL_NAME(move16_dst),r(a0)
	/* move16	SYMBOL_NAME(move16_src),r(a0)@ */
	.word	0xf618	
	.long	SYMBOL_NAME(move16_src)	

SYMBOL_NAME(cont_check_cpu):
	/* Restore old vbr vectors */
	movec	r(vbr),r(a0)
	movel	r(sp)@+,r(a0)@(F_LINE_EMU)
	movel	r(sp)@+,r(a0)@(ILLEGAL_INSTR)

	/* Enable Interrupts */
	movew	r(sp)@+,r(sr)

	/* set return value */
	movel	r(pc)@(SYMBOL_NAME(tmp)),r(d0)

	moveml	r(sp)@+,r(a0)/r(a1)
	rts


/*
 * displays char taken from the stack
 */
SYMBOL_NAME(putc):
	moveml	r(d0)-r(d7)/r(a0)-r(a6),r(sp)@-
	movel	r(sp)@(64),r(sp)@-
	jsr	SYMBOL_NAME(console_putc)
	addqw	#4,r(sp)
	moveml	r(sp)@+,r(d0)-r(d7)/r(a0)-r(a6)
	movel	r(sp)@,r(sp)@(4)
	addqw	#4,r(sp)
	rts

SYMBOL_NAME(putr):	
	/* movel	#'\n',%sp@-  NeXT doesn't allow #'x' */
	movel	#10,r(sp)@-
	jsr	SYMBOL_NAME(putc)
	rts

SYMBOL_NAME(putn):
	moveml	r(d0)-r(d7)/r(a0)-r(a6),r(sp)@-
	movel	r(sp)@(64),r(sp)@-
	jsr	SYMBOL_NAME(console_putnum)
	addqw	#4,r(sp)
	moveml	r(sp)@+,r(d0)-r(d7)/r(a0)-r(a6)
	movel	r(sp)@,r(sp)@(4)
	addqw	#4,r(sp)
	rts

