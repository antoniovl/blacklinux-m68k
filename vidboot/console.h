#ifndef __CONSOLE_H__

#define putr()	console_putc('\n');
#define putsep1()	console_puts(", ");
#define putsep2()	console_puts(" - ");
#define putsep3()	console_puts("-->");

#define FONT_8x8	0
#define FONT_8x16	1

/* As console_plot_pixel() it's used only by console_put_penguin
   and console_putc, this function should not be accessble from
   main.c, but included just in case. */
int console_init();
int console_clear();
int console_plot_pixel( unsigned long, unsigned long, unsigned long );
int console_put_penguin( int, int );
int console_scroll();
int console_putc( char );
int console_puts( char * );
int console_putnum( unsigned long );
int console_putnum10( unsigned long );
int console_gotoxy( unsigned, unsigned );
int console_getc();
int console_getchar();
char *console_gets( char * );

#define __CONSOLE_H__
#endif