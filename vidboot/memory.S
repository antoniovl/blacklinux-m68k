/*
 *	Size NeXT Memory
 *
 * This algorithm will determine the amount of physical RAM
 * present in a NeXT workstation and determine it's exact addresses.
 * This algorithm works on the following assumptions:
 * 1) The beginning of physical RAM is always at 0x04000000
 * 2) There is no RAM at or above 0x0C000000
 * 3) The smallest unit of memory in the system is a Megabyte
 * 4) There are regions of shadow RAM that look like the RAM before
      the shadow region.
 * 5) Shadow regions always go until the next valid RAM
 *
 * The algorithm works by clearing memory locations and then
 * (beginning at low memory) begins modifying the memory and looking
 * forward to the next megabyte to see if the modification has
 * propogated forward.  Propogated changes are signs of "Shadow"
 * RAM.  That is, there isn't any RAM present there, but a shadow
 * of already seen RAM.  As long as the shadow continues (moving
 * forward) Shadow RAM is deemed to persist.  As soon as either
 * RAM is found (cleared, not modified) then we're back into real
 * RAM and it is added to the list of regions.
 *
 * There are two reasons why this routine cannot be written in C:
 * 1) C routines may be arbitrarily large and are less carefully placed
 *    in memory, so the routine may in advertantly modify itself during
 *    the test (as it modifies memory looking for real RAM).
 * 2) C routines often use the stack and there is no gaurantee that the
 *    stack will not be affected by this routine.
 *
 * Therefore, the routine is written in assembler so that:
 * 1) Its placement within the program can be fixed.  It must not cross
 *    a megabyte boundary.
 * 2) It only uses registers, except for some PC relative memory which
 *    must also -NOT- cross a megabyte boundary.
 *
 * This routine should be passed a ptr on the stack into which
 * the results from this routine will be placed.  The calling
 * convention of this routine is:
 * void size_memory(struct memory_descriptor *p);
 *
 * struct memory_chunk {
 *	void		*start;
 *	unsigned long	 length;
 * }				;
 * 
 * struct memory_descriptor {
 *	unsigned long		count;
 *	struct memory_chunk	mem[8];
 * };
 *
 * The proper way to read the structure is that mem[0] is always known
 * to be valid.  count represents the number of mem[] chunks which are
 * valid [0..count-1].
 */

#include "asm-setup.h"

	/* Size Memory */
#define MEM_START	0x04000000
#define MEM_END		0x0C000000
#define MEM_MB		0x00100000
#define LIST_SIZE	64
#define SAVE_SIZE	512
#define elem_addr	0
#define elem_size	4


#define SAVE_PTR	r(a5)
#define LIST_PTR	r(a4)
#define MEM_PTR		r(a3)
#define STRUCT_PTR	r(a2)
#define MEM_TOP_PTR	r(a1)

#define INDEX		r(d6)
#define COUNTER		r(d5)
#define TEMP		r(d4)
#define SHIFT		r(d3)
#define REG_LIST	 LIST_PTR\
			/SAVE_PTR\
			/INDEX/MEM_PTR\
			/MEM_TOP_PTR\
			/COUNTER\
			/STRUCT_PTR\
			/TEMP\
			/SHIFT

.globl	SYMBOL_NAME(size_memory)

/* This routines now reside in asm-tools.S 
.globl	SYMBOL_NAME(putn)
.globl	SYMBOL_NAME(putr)*/ 

/* dirty debugging */
/* #define DBG */
	
.text
		
	/*
	 * I once said to myself, oh, this list_storage doesn't
	 * need to be here because I've got the place on the stack
	 * into which I can write the list information.
	 * No.
	 * That won't work because the stack version may cross
	 * a Megabyte boundary and that would result in	trashed
	 * data.
	 * So, this is needed.
	 */
list_storage:
	.long	0,0
	.long	0,0
	.long	0,0
	.long	0,0
	.long	0,0
	.long	0,0
	.long	0,0
	.long	0,0

	/* This is where we store the RAM data before we
	 * alter it.
	 */
save_storage:
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0
	.long	0,0,0,0,0,0,0,0

#define	A0	r(a0)
#define	A1	r(a1)
#define	A2	r(a2)
#define	A3	r(a3)
#define	A4	r(a4)
#define	A5	r(a5)
#define	A6	r(a6)
#define	A7	r(a7)
#define	SP	r(sp)

#define	D0	r(d0)
#define	D1	r(d1)
#define	D2	r(d2)
#define	D3	r(d3)
#define	D4	r(d4)
#define	D5	r(d5)
#define	D6	r(d6)
#define	D7	r(d7)
	
#define PC	r(pc)
#define SR	r(sr)
#define VBR	r(vbr)

SYMBOL_NAME(save_sp):
	.long	0
	
SYMBOL_NAME(bus_error):
#ifdef	DBG
	movel	#0x0000ffff,r(sp)@-
	jsr	SYMBOL_NAME(putn)
#endif
	movel	SYMBOL_NAME(save_sp),SP			
	movel	MEM_PTR,MEM_TOP_PTR
	jmp	SYMBOL_NAME(found_mem_end)

SYMBOL_NAME(addr_error):
#ifdef	DBG
	movel	#0x0000eeee,r(sp)@-
	jsr	SYMBOL_NAME(putn)
#endif
	movel	SYMBOL_NAME(save_sp),SP			
	movel	MEM_PTR,MEM_TOP_PTR
	jmp	SYMBOL_NAME(found_mem_end)


SYMBOL_NAME(size_memory):
	link	A6,#0
	moveml	REG_LIST,SP@-

#ifdef DBG
	/* This flags the entering to size_memory() */
	movel	#0xaaaaaaaa,r(sp)@-
	jsr	SYMBOL_NAME(putn)
#endif

	/* Disable interrupts so we don't
	 * punch through an invalid VBR entry
	 */
	movew	SR,SP@-
	oriw	#0x0700,SR

	/* Save NeXT's BUS ERROR handler */
#ifdef	DBG
	movel	#0x1,r(sp)@-
	jsr	SYMBOL_NAME(putn)
#endif
	movec	VBR,A0
	movel	A0@(8),SP@-

	/* Save addr error handler --antonio */
#ifdef	DBG
	movel	#0x2,r(sp)@-
	jsr	SYMBOL_NAME(putn)
#endif
	movel	r(a0)@(0xc),r(sp)@-

	/* Load our custom BUS ERROR handler */
	movel	#SYMBOL_NAME(bus_error),A0@(8)
	/* Load the same handler into addr vector --antonio */
	movel	#SYMBOL_NAME(addr_error),r(a0)@(0xc)

	/* Save the SP in case of a bus error */
	movel	SP,SYMBOL_NAME(save_sp)

	/* Clear the List */
	lea	PC@(list_storage),LIST_PTR
	movel	#LIST_SIZE/8-1,INDEX

1:	clrl	LIST_PTR@(elem_addr,INDEX:l:8)
	clrl	LIST_PTR@(elem_size,INDEX:l:8)
	dbra	INDEX,1b

	/* Save all the bytes we might affect */
	lea	PC@(save_storage),SAVE_PTR
	clrl	INDEX
	moveal	INDEX,MEM_TOP_PTR
	lea	MEM_START,MEM_PTR
1:
#if defined(DBG)
	pea	MEM_PTR@
	jsr	SYMBOL_NAME(putn)
	jsr	SYMBOL_NAME(putr)
#endif
	movel	MEM_PTR@,D0		/* Attempt to get RAM data */
	movel	D0,SAVE_PTR@(0,INDEX:l:4) /* Save the RAM data */
	clrl	MEM_PTR@		/* CLR RAM */
	addql	#1,INDEX
	addal	#MEM_MB,MEM_PTR
	cmpal	#MEM_END,MEM_PTR	/* Have we reached the END? */
	bne	1b			/* No, continue */

	moveal	#MEM_END,MEM_TOP_PTR	/* Use an absolute END */

SYMBOL_NAME(found_mem_end):		

#ifdef	DBG
	/* this flags that we're back from the exeption */
	jsr	SYMBOL_NAME(putr)
	movel	#0x55550000,r(sp)@-
	jsr	SYMBOL_NAME(putn)
	jsr	SYMBOL_NAME(putr)
#endif

	/* Begin Memory test */
	clrl	INDEX
	lea	MEM_START,MEM_PTR
1:
		/* Create a list entry for this address range */
		/* Note: we assume that our MEM_PTR always BEGINS */
		/* pointing at real RAM */
	movel	MEM_PTR,LIST_PTR@(elem_addr,INDEX:l:8)
	clrl	LIST_PTR@(elem_size,INDEX:l:8)
2:
		/* Are we done? */
	cmpal	MEM_TOP_PTR,MEM_PTR
	beq	4f		/* DONE */

		/* Test the contents of this address */
		/* If we see a zer0 we know we're looking at RAM */
		/* Otherwise, we know that a RAM data value from */
		/* a previous RAM bank is being shadowed forward */
		/* and that's what we see here */
	tstl	MEM_PTR@
	bne	3f		/* Found Shadow */

		/* We flag a VALID RAM bank with 1 so that forward */
		/* references will see the 1 and we'll know we're */
		/* no longer in RAM */	
	addql	#1,MEM_PTR@	/* Mark as RAM */
	addal	#MEM_MB,MEM_PTR
	addql	#1,LIST_PTR@(elem_size,INDEX:l:8)
	bra	2b		/* Still in RAM */
	
	/* Hit shadow RAM */
	/* Walk through shadow RAM until we bump into a zero */
	/* Then we'll know we're in real RAM again */
3:
	addal	#MEM_MB,MEM_PTR
	cmpal	MEM_TOP_PTR,MEM_PTR
	beq	4f		/* DONE */
	tstl	MEM_PTR@
	bne	3b		/* Still in Shadow */

	/* Hit RAM Again */
	addql	#1,INDEX
	jbra	1b

	/* DONE */
4:	
	/* Restore RAM Values */
	/* Note: Only restore values that are in RAM! */
	/* Restoring Shadow would be -- at best benign, */
	/* At worst, destructive. */
	clrl	INDEX
	lea	MEM_START,MEM_PTR
4:
	cmpal	MEM_TOP_PTR,MEM_PTR	/* Are we done? */
	beq	9f
		/* Are we at the address yet? */
	cmpl	LIST_PTR@(elem_addr,INDEX:l:8),MEM_PTR
	beq	5f	/* Yes */
	addqw	#4,SAVE_PTR	/* Don't restore from this SAVE_PTR */
	addal	#MEM_MB,MEM_PTR	/* Next RAM address */
	jbra	4b
5:
		/* Get the size -1 (68K DBRA instr) */
	movel	LIST_PTR@(elem_size,INDEX:l:8),COUNTER
	subql	#1,COUNTER
6:
	movel	SAVE_PTR@+,MEM_PTR@	/* Restore data */
	addal	#MEM_MB,MEM_PTR		/* 
	dbra	COUNTER,6b
	addql	#1,INDEX
	jbra	4b

	/* Copy the results up to the passed parameter */
9:	
	clrl	INDEX
	movel	A6@(8),STRUCT_PTR
	addaw	#4,STRUCT_PTR	/* Point to mem[0] */
	movew	#20,SHIFT
1:
	movel	LIST_PTR@+,STRUCT_PTR@+
	movel	LIST_PTR@+,TEMP
	asll	SHIFT,TEMP
	movel	TEMP,STRUCT_PTR@+	/* Turn # of megs into # bytes */
	addql	#1,INDEX
	cmpl	#0,TEMP			/* Did we come across a zero count? */
	bne	1b

	movel	A6@(8),STRUCT_PTR
	movel	INDEX,STRUCT_PTR@

	/* Restore VBR */
	movec	VBR,A0
	movel	r(sp)@+,r(a0)@(0xc)
	movel	SP@+,A0@(8)

	/* Restore SR */
	movew	SP@+,SR

#ifdef	DBG
	/* This marks the end of size_memory */
	movel	#0xafafafaf,r(sp)@-
	jsr	SYMBOL_NAME(putn)
	jsr	SYMBOL_NAME(putr)
#endif

	moveml	SP@+,REG_LIST
	unlk	A6
	rts
