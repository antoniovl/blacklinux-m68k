/*
 * Debug routines implementation.
 */
#include "debug.h"
#include "string.h"
#include "console.h"

/* Variables loaded at boot time.
   Declared in main_setup.c */
extern unsigned long reg_d0, reg_d1, reg_d2, reg_d3,
	reg_d4, reg_d5, reg_d6, reg_d7, 
	*reg_a0, *reg_a1, *reg_a2, *reg_a3, *reg_a4, 
	*reg_a5, *reg_a6, *reg_a7;


int pointer_dump( unsigned long *p, unsigned long n )
{
 unsigned long i, j, k, offset;
 
	console_puts(" pointer_dump(): Dumping ");
	console_putnum(n);
	console_puts(" unsigned longs at ");
	bcopy(&((unsigned long *)p),&k,sizeof(unsigned long));
	console_putnum(k);
	k = n*sizeof(unsigned long);
	console_puts(", ");
	console_putnum(k);
	console_puts(" bytes [offset(bytes):addr:value]:");
	putr();
	
	k=0;
	console_putc(' ');
	for (i=0; i<n; i++) {
		offset = i*sizeof(unsigned long);
		console_putnum(offset);
		console_putc(':');
		console_putnum((unsigned long)&p[i]);
		console_putc(':');
		console_putnum(p[i]);
		if (k==3) {
			console_puts("\n ");
			k=0;
		} else {
			putsep2();
			k++;
		}
	}
	
	return 0;
}



int register_dump()
{
	console_puts(" register_dump():\n");
	console_puts("\t%d0: "); console_putnum(reg_d0); putsep2();
	console_puts("\t%d1: "); console_putnum(reg_d1); putsep2();	
	console_puts("\t%d2: "); console_putnum(reg_d2); putsep2();	
	console_puts("\t%d3: "); console_putnum(reg_d3); putr();	
	console_puts("\t%d4: "); console_putnum(reg_d4); putsep2();	
	console_puts("\t%d5: "); console_putnum(reg_d5); putsep2();	
	console_puts("\t%d6: "); console_putnum(reg_d6); putsep2();	
	console_puts("\t%d7: "); console_putnum(reg_d7); putr();	
	console_puts("\t%a0: "); console_putnum((unsigned long)reg_a0); putsep1(); 
	console_putnum(*reg_a0); putsep3();
	console_puts("\t%a1: "); console_putnum((unsigned long)reg_a1); putsep1(); 
	console_putnum(*reg_a1); putr();	
	console_puts("\t%a2: "); console_putnum((unsigned long)reg_a2); putsep1(); 
	console_putnum(*reg_a2); putsep3();
	console_puts("\t%a3: "); console_putnum((unsigned long)reg_a3); putsep1(); 
	console_putnum(*reg_a3); putr();
	console_puts("\t%a4: "); console_putnum((unsigned long)reg_a4); putsep1(); 
	console_putnum(*reg_a4); putsep3();
	console_puts("\t%a5: "); console_putnum((unsigned long)reg_a5); putsep1(); 
	console_putnum(*reg_a5); putr();
	console_puts("\t%a6: "); console_putnum((unsigned long)reg_a6); putsep1(); 
	console_putnum(*reg_a6); putsep3();
	console_puts("\t%a7: "); console_putnum((unsigned long)reg_a7); putsep1(); 
	console_putnum(*reg_a7); putr();	
	return 0;
}