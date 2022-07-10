/*
 * console.c
 * Console routines implementation
 * (c) 1998 - antonio@naiki.com
 * 032598: Updated with Randy's corrections
 * 032698: Color support thanks to Timm Wetzel (twetzel@cage.mpibpc.gwdg.de)
 */
#include "console.h"
#include "font_8x16.h"
#include "font_8x8.h"
#include "main_setup.h"

/* names taken from bootinfo structure */
unsigned long BI_videoaddr,      /* FB video addr */
    BI_videorow, BI_videoheight, /* Height of screen in pixels */
    BI_videowidth,               /* Width of screen in pixels */
    BI_dimensions, BI_nppw,      /* Number of pixel per word */
    BI_nbpp,                     /* Number of bits per pixel */
    BI_colormask,                /* Color Mask */
    BI_white,                    /* White color definition */
    BI_lt_gray,                  /* Light gray definition */
    BI_dk_gray,                  /* Dark gray definition */
    BI_black;                    /* Black color definition */

/* Console data structures */
struct console_globals {
  long column;
  long row;
  long max_num_cols;
  long max_num_rows;
  long left_edge;
  long mac_putc;
} console_globals;

struct console_font_dimensions {
  long fontwidth;
  long fontheight;
} console_font_dimensions;

char that_penguin[] = {
#include "that_penguin.h"
};

/* Local variable (to this file). This holds
   either fontdata_8x8 or fontdata_8x16 */
static unsigned char *fontdata;

/* bg & fg colors */
static unsigned long console_bg, console_fg;

int inc_row();
int inc_col();

/* working buffer */
char buffer[1024];

/* declared in console.h */
static short int font_type; /* Values: FONT_8x8, FONT_8x16 */

/* Pointer to video memory */
void *video;

#define TABSIZE 8
#define CFD_FW console_font_dimensions.fontwidth
#define CFD_FH console_font_dimensions.fontheight

/* Keyboard hack */
int (*mon_getc)();
int (*mon_putc)();

int console_clear() {
  void *vptr;
  unsigned int i, words_per_row;
  long wordmask, wm1;

  /* As 32 bit (1 long) transfers are faster than
     8 bits (1 char) ones, when working with monos we need
     to build a 32 bit wordmask to be copied to video memory
     as long words. */
  wordmask = 0;
  for (i = 0; i <= 32 - BI_nbpp; i += BI_nbpp) {
    wordmask |= ((console_bg & BI_colormask) << i);
  }

  /* We're accessing video memory as long words */
  vptr = (unsigned long *)BI_videoaddr;

  /* BI_videowidth pixels / number of pixel per word equals
     number of 32 bit words in a row */
  words_per_row = (BI_videowidth / BI_nppw);

  /* there are BI_videoheight rows */
  for (i = 0; i < (words_per_row * BI_videoheight); i++) {
    *(unsigned long *)vptr = (unsigned long)wordmask;
    ((unsigned long *)vptr)++;
  }

  return 0;
}

int console_init() {
  long i;

  /* this must be fixed */
  mon_getc = mg->mg_getc;
  mon_putc = mg->mg_putc;

  /* Set bg & fg colors.
     Must be first than initial
     console_clear() */
  console_bg = BI_black;
  console_fg = BI_white;

  /* font_type = FONT_8x8; */
  /* let's try this, looks much better */
  font_type = FONT_8x16;

  switch (font_type) {
  case FONT_8x8:
    console_font_dimensions.fontheight = 8;
    console_font_dimensions.fontwidth = 8;
    fontdata = (unsigned char *)&fontdata_8x8[0];
    break;
  case FONT_8x16:
    console_font_dimensions.fontheight = 16;
    console_font_dimensions.fontwidth = 8;
    fontdata = (unsigned char *)&fontdata_8x16[0];
    break;
  default:
    break;
  }

  console_globals.max_num_cols = BI_videowidth / CFD_FW;
  console_globals.max_num_rows = BI_videoheight / CFD_FH;
  console_globals.column = 0;
  console_globals.row = 0;

  console_clear();

  return 0;
}

int console_plot_pixel(unsigned long x, unsigned long y, unsigned long color) {
  unsigned long bit_num, *lvptr, mask;
  unsigned char *vptr, bit_data, c1;

  switch (BI_nppw) {
  case 16: /* mono framebuffer */
    /* Note: In this case, a "word" is handled by
       a char, so, BI_nppw it's the number of
       pixels in a 8 bit (sizeof(char)) word.
       That's why you're reading (x/4). */
    vptr = (unsigned char *)BI_videoaddr + (x / 4) + y * BI_videorow;
    bit_num = ((x & 3) ^ 3) << 1;
    c1 = (unsigned char)color;
    c1 &= 1;
    bit_data = (1 << (bit_num + 1)) | (1 << bit_num);
    if (!c1) {
      /* white pixel */
      *vptr &= ~bit_data;
    } else {
      /* black pixel */
      *vptr |= bit_data;
    }
    break;
  case 2: /* 16 bit color version */
    lvptr = (unsigned long *)BI_videoaddr + (x / BI_nppw) +
            y * BI_videowidth / BI_nppw;
    mask = ((x % BI_nppw) == 0) ? 0x0000ffff : 0xffff0000;
    /* Let's see if we can plot colors */
    /* *lvptr = (*lvptr & !mask) |
                    (mask & (color ? BI_black : BI_white)); */
    *lvptr = ((*lvptr & !mask) | (mask & color));
    break;
  case 1: /* 32 bit color NeXTdimension board */
    lvptr = (unsigned long *)BI_videoaddr + (x / BI_nppw) +
            (y * BI_videowidth / BI_nppw);
    *lvptr = (unsigned long)color;
    break;
  default:
    break;
  }
  return 0;
}

int console_put_penguin(int x, int y) {
  int j = 0, i = 0, k = 0;
  unsigned char *p = (unsigned char *)&that_penguin[0];

  for (j = 0; j < 74; j++) {
    for (i = 0; i < 64; i++) {
      console_plot_pixel(i + x, j + y, (*p) >> 4);
      i++;
      console_plot_pixel(i + x, j + y, *p);
      *p++; /* why not just p++? */
    }
  }
  return 0;
}

int console_scroll() {
  unsigned long *lptr0, *lptr1, words_row, ini_row, end_row, i, j;

  ini_row = console_font_dimensions.fontheight;
  end_row = BI_videoheight;
  words_row = BI_videowidth / BI_nppw;
  lptr0 = (unsigned long *)BI_videoaddr;
  lptr1 = (unsigned long *)BI_videoaddr + words_row * ini_row;
  for (i = ini_row; i <= end_row; i++) {
    /* let's move long words (faster) */
    for (j = 0; j <= words_row; j++) {
      lptr0[j] = lptr1[j];
    }
    lptr0 += words_row;
    lptr1 += words_row;
  }
  /* now clear the last row */
  ini_row = BI_videoheight - console_font_dimensions.fontheight;
  lptr0 = (unsigned long *)BI_videoaddr + words_row * ini_row;
  for (i = ini_row; i <= end_row; i++) {
    for (j = 0; j <= words_row; j++)
      lptr0[j] = console_bg;
    lptr0 += words_row;
  }
  return 0;
}

int inc_row() {

  /* this routine will call console_scroll */
  if ((console_globals.row + 2) >= console_globals.max_num_rows) {
    /* we need scrolling */
    console_scroll();
  } else {
    console_globals.row++;
  }
  return 0;
}

int inc_col() {
  if ((console_globals.column + 1) >= console_globals.max_num_cols) {
    /* llegamos al extremo derecho de la pantalla */
    inc_row();
    console_globals.column = 0;
  } else {
    console_globals.column++;
  }
  return 0;
}

int console_putc(char c) {
  unsigned char k, a;
  int i, j, x1, y1;

  /* Do we need to scroll? */
  if (c == '\n') {
    inc_row();
    console_globals.column = 0;
    return 0;
  }

  if (c == '\r') {
    console_globals.column = 0;
    return 0;
  }

  x1 = console_globals.column * CFD_FW;
  y1 = console_globals.row * CFD_FH;
  for (i = 0; i < CFD_FH; i++) {
    k = fontdata[c * CFD_FH + i];
    for (j = 0; j < CFD_FW; j++) {
      a = k & (1 << (CFD_FW - j));
      /* quitando el else no se plotea el background
         blanco, la letra se escribe sobre el color
         actual de la pantalla */
      if (a) {
        /* console_plot_pixel((j+x1),(i+y1),1); */
        console_plot_pixel((j + x1), (i + y1), console_fg);
      } else {
        /* Temporary left out, so we can have a
         faster char plotting */
        /* console_plot_pixel((j+x1),(i+y1),0); */
        /* console_plot_pixel((j+x1),(i+y1),console_bg); */
      }
    }
  }
  inc_col();
  return 0;
}

int console_puts(char *string) {
  int i, j;
  i = 0;
  while (string[i] != 0) {
    switch (string[i]) {
    case '\t':
      for (j = 0; j < TABSIZE; j++)
        console_putc(' ');
      break;
    default:
      console_putc(string[i]);
      break;
    }
    i++;
  }
  return (i);
}

int console_putnum(unsigned long n) {
  char c, d;
  int i;

  console_puts("0x");

  for (i = 28; i >= 0; i -= 4) {
    d = (n >> i) & 15;
    c = d > 9 ? d + 'a' - 10 : d + '0';
    console_putc(c);
  }

  return 0;
}

int reverse_string(const char *src, char *dst) {
  register int i, j;
  j = 0;
  for (i = strlen(src) - 1; i >= 0; i--) {
    dst[j++] = src[i];
  }
  dst[j] = 0;
  return (j);
}

int console_putnum10(unsigned long n) {
  char c, d;
  int done, i;
  unsigned long quot, rem, curr;
  char digits[10] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
  char number[255];

  if (n < 10) {
    console_putc(digits[n]);
    return 0;
  }

  i = 0;
  done = 0;
  quot = n;
  bzero(number, 255);
  bzero(buffer, 1024);

  while (!done) {
    curr = quot;
    quot = curr / (unsigned long)10;
    rem = curr % (unsigned long)10;
    if (quot < (unsigned long)10) {
      number[i++] = digits[rem];
      number[i++] = digits[quot];
      done = 1;
    } else {
      number[i++] = digits[rem];
    }
  }

  number[i] = 0;
  reverse_string(number, buffer);
  console_puts(buffer);
  return 0;
}

int console_gotoxy(unsigned x, unsigned y) {
  if ((x < console_globals.max_num_cols) &&
      (y < console_globals.max_num_rows)) {
    console_globals.column = (long)x;
    console_globals.row = (long)y;
  }
  return 0;
}

int console_getc() {
  /* this must be fixed */
  return ((int)mon_getc());
}

int console_getchar() {
  register char c;
  c = mon_getc();
  console_putc(c);
  return ((int)c);
}

char *console_gets(char *s) {
  register int i;
  int done;

  /* there is no check of buffer overruns */
  done = 0;
  i = 0;
  while (!done) {
    s[i] = mon_getc();
    switch (s[i]) {
    case '\n':
    case '\r':
      s[i] = 0;
      done = 1;
      break;
    default:
      console_putc(s[i]);
      i++;
      break;
    }
  }
  return (s);
}