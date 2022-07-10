/*
 * Routines written in asm, visible to C sources.
 */
#ifndef __ASM_TOOLS_H__

#include "next/next-mon-global.h"

extern unsigned short get_sr(); /* sr is a 16 bit register */
extern void enable_interrupts();
extern void disable_interrupts();
extern unsigned int get_cacr();
extern unsigned int get_vbr();

extern unsigned long check_cpu();

extern void linux_macho_launch();
extern void linux_rawbin_launch();
unsigned long get_end_addr();
extern void get_mon();

extern unsigned putc( char );
extern unsigned putr();
extern unsigned putn( unsigned );

#define __ASM_TOOLS_H__
#endif