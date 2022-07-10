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