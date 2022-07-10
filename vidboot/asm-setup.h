#ifndef __SETUP_H__

/* #if defined(ASSEMBLY) */

#ifdef __NeXT_BUILD
#  define r(x)	x
#  define SYMBOL_NAME(x)	_##x
#  define jbra	bra
#  define CACHELINE_ALIGNMENT	4
#else
#  define r(x)	%##x
#  define SYMBOL_NAME(x)	x
#  define CACHELINE_ALIGNMENT	16
#endif

/* #endif */

#define __SETUP_H__
#endif