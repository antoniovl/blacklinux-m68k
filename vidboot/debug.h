/*
 * Debugging Routines.
 */
#ifndef __DEBUG_H__

/* Dumps n unsigned longs from pointer *p */
int pointer_dump( unsigned long *p, unsigned long n);
int register_dump();

#define __DEBUG_H__
#endif