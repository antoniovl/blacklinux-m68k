#include "asm-tools.h"
#include "console.h"
#include "debug.h"
#include "main_setup.h"
#include "memory.h"
#include "next/next-mon-global.h"
#include "string.h"
#include "vbr.h"

#include "image.h"

#define INT_OFF 0xf8ff
#define TRAP_OFF 0x3ffff
#define SUP_BIT 0x2000
#define ETHER_ADDR 0x8

#define REV "Revision 3.0"

short int sr0, sr1;

struct scb vbr_table;

/* 042698: for testing : */
extern unsigned long reg_d0, reg_d1;

/* Variables de la NeXT */
extern struct machine_config NeXT_cfg; /* Declared in main_setup.c */
unsigned int slot_id, slot_id_bmap;
unsigned long cacr;
/*unsigned char cpu_rev, machine_type,
         cpu_type, dma_rev, cpu_clock,
         board_rev, videomem_speed,
         mainmem_speed;*/
/* struct scr1 scr1;*/
struct mon_global mon_global, *mg1; /* test */
struct km_console_info kmci;        /* test */
/* The following ones will hold the parameters to m68k_init
   as stated in sdmach.S */
unsigned long *ultmp, num_regions, eth_addr[2];
char buffer[1024], str_eth_addr[16];

/* Linux Vars */
unsigned char
    *linux_start_addr, /* Holds start addr of kernel image, untouchable.
                          This way can be accessed from asm-tools.S */
    *dst,              /* TmpVar for Destination address of kernel */
    *img;              /* TmpVar image data */

/* int main( struct mon_global *mg, unsigned long a0 )*/
int main(unsigned long *arg) {
  char *tmpstr, c;
  unsigned long i, j, k;
  struct memory_list memory;

  /* The *mg (mon_global) struct has been loaded in
     _start */

  /* Setup of the machine. Has to be first.
     Uses a very simple heuristic. */
#if defined(DO_MONOCHROME_VIDEO)
  machine_setup(VIDEO_MONO);
#else
  machine_setup(VIDEO_COLOR);
#endif

  /* Fetch the status register */
  sr0 = get_sr();

  console_init();

  console_put_penguin(1000, 0);

  console_puts("NeXT Linux Console Debugger.\n");
  console_puts(
      "Comments, patches & bugfixes to Antonio Varela (antonio@naiki.com)\n");
  console_puts(" - Console routines based on Randy Thelen's "
               "(rthelen@ix.netcom.com) head.S\n");
  console_puts(
      " - Color support thanks to Timm Wetzel (twetzel@cage.mpibpc.gwdg.de)\n");
  putr();

  console_puts(REV);

  console_puts("\nRun-Time Options:\n - Machine is a ");
  console_puts(machine_string);
  putr();

#if defined(DO_KEYB_TEST)
  console_puts("press any key ...");
  c = console_getc();
  console_puts("\nand it was: '");
  console_putc(c);
  console_puts("' - hex value of this char: ");
  console_putnum(c);
  console_puts("\nHit a key again (Now with console_getchar): ");
  c = console_getchar();
  console_puts("\nEnter a string (spaces allowed): ");
  console_gets(buffer);
  console_puts("\nString was: ");
  console_puts(buffer);
  console_puts("\nboot_dev: '");
  console_puts(mg->mg_boot_dev);
  console_puts("', boot_arg: '");
  console_puts(mg->mg_boot_arg);
  console_puts("', boot_info: '");
  console_puts(mg->mg_boot_info);
  console_puts("', boot_file: '");
  console_puts(mg->mg_boot_file);
  console_puts("'.");
#endif

#if defined(SHOW_KM_CONSOLE_INFO)
  bcopy((void *)&(mg->km_coni), (void *)&kmci, sizeof(kmci));
  console_puts("\nStruct km_console_info: ");
  console_puts("\n\tpixels_per_word: ");
  console_putnum10(kmci.pixels_per_word);
  console_puts("\n\tdspy_w: ");
  console_putnum10(kmci.dspy_w);
  console_puts("\n\tFramebuffer addr: ");
  console_putnum(kmci.map_addr[KM_CON_FRAMEBUFFER].phys_addr);
#endif

#if defined(DO_SIZEMEM)
  console_puts("\nSizing Memory...");
  size_memory(&memory);
  console_puts("Ok.\n");
  k = 0;
  for (i = 0; i < memory.count; i++) {
    if (memory.mem[i].size) {
      k = k + memory.mem[i].size;
      console_puts("\tBank ");
      console_putnum10(i);
      console_puts(" at ");
      console_putnum((unsigned long)memory.mem[i].addr);
      console_puts(" - ");
      console_putnum10(memory.mem[i].size);
      console_puts(" Bytes");
      putr();
    }
  }
  console_puts("\tMemory Size is ");
  console_putnum10(k / 1000000);
  console_puts(" Mb [");
  console_putnum10(k);
  console_puts(" Bytes].");
  putr();
#endif

#if defined(SHOW_CONSOLE_SETTINGS)
  /* Show console settings */
  console_puts("\nConsole Settings: \n");
  console_puts(" - BI_videoaddr  : ");
  console_putnum(BI_videoaddr);
  putr();
  console_puts(" - BI_videowidht : ");
  console_putnum(BI_videowidth);
  putr();
  console_puts(" - BI_videoheight: ");
  console_putnum(BI_videoheight);
  putr();
  console_puts(" - Video Memory Speed: ");
  switch (NeXT_cfg.scr1.videomem_speed) {
  case MEM_120NS:
    tmpstr = "120ns\n";
    break;
  case MEM_100NS:
    tmpstr = "100ns\n";
    break;
  case MEM_80NS:
    tmpstr = "80ns\n";
    break;
  case MEM_60NS:
    tmpstr = "60ns\n";
    break;
  default:
    tmpstr = "Unknown Speed.\n";
    break;
  }
  console_puts(tmpstr);
  putr();
#endif

  /* Show CPU Settings */
  console_puts("\nCPU & Memory Settings: \n");
  console_puts("- CPU is a ");
  switch (NeXT_cfg.cpu_clock) {
  case CPU_40MHZ:
    tmpstr = "40Mhz ";
    break;
  case CPU_33MHZ:
    tmpstr = "33Mhz ";
    break;
  case CPU_25MHZ:
    tmpstr = "25Mhz ";
    break;
  case CPU_20MHZ:
    tmpstr = "20Mhz ";
    break;
  default:
    break;
  }
  console_puts(tmpstr);

  tmpstr = (NeXT_cfg.cpu_type == M68040) ? "MC68040 " : "MC68030 ";
  console_puts(tmpstr);

  /* show the machine_type as in /usr/include/next/scr.h */
  switch (NeXT_cfg.machine_type) {
  case NeXT_CUBE:
    tmpstr = "(NeXT_CUBE), ";
    break;
  case NeXT_WARP9:
    tmpstr = "(NeXT_WARP9), ";
    break;
  case NeXT_X15:
    tmpstr = "(NeXT_X15), ";
    break;
  case NeXT_WARP9C:
    tmpstr = "(NeXT_WARP9C), ";
    break;
  default:
    console_puts("(Code: ");
    console_putnum(NeXT_cfg.machine_type);
    tmpstr = " -- Unknown Encoding!), ";
    break;
  }
  console_puts(tmpstr);

  /* show CPU status */
  if (sr0 & SUP_BIT)
    console_puts("running in supervisor mode [%sr = ");
  else
    console_puts("running in user mode [%sr = ");
  console_putnum((unsigned long)sr0);
  console_puts("].\n");

#if defined(SHOW_MEMSPEED)
  /* Show memory speed, not reliable data yet */
  console_puts("- Main Memory Speed is ");
  switch (NeXT_cfg.mainmem_speed) {
  case MEM_120NS:
    tmpstr = "120ns.\n";
    break;
  case MEM_100NS:
    tmpstr = "100ns.\n";
    break;
  case MEM_80NS:
    tmpstr = "80ns.\n";
    break;
  case MEM_60NS:
    tmpstr = "60ns.\n";
    break;
  default:
    tmpstr = "(Unknown Speed).\n";
    break;
  }
  console_puts(tmpstr);
#endif

  /*
          cacr = get_cacr();
          console_puts("- %cacr: ");
          console_putnum(cacr);
          console_puts(".\n\n");
  */

#if defined(SHOW_SCR1)
  /* Show data collected */
  console_puts("NeXT's Internal Data: \n");
  console_puts(" - slot_id: ");
  console_putnum(NeXT_cfg.slot_id);
  putr();

  console_puts(" - System Control Register 1 (scr1): ");
  bcopy(&NeXT_cfg.scr1, &cacr, sizeof(struct scr1));
  console_putnum(cacr);
  putr();
  console_puts("\tdma_rev: ");
  console_putnum(NeXT_cfg.dma_rev);
  putr();
  console_puts("\tcpu_rev: ");
  console_putnum(NeXT_cfg.cpu_rev);
  putr();
  console_puts("\tmachine_type: ");
  console_putnum(NeXT_cfg.machine_type);
  putr();
  console_puts("\tboard_rev: ");
  console_putnum(NeXT_cfg.board_revision);
  putr();
  console_puts("\tCPU clock: ");
  console_putnum((unsigned long)NeXT_cfg.cpu_clock);
  putr();
  console_puts("\tvideomem_speed: ");
  console_putnum(NeXT_cfg.videomem_speed);
  putr();
  console_puts("\tmainmem_speed: ");
  console_putnum(NeXT_cfg.mainmem_speed);
  putr();
#endif

#if 0
	cacr = get_vbr();
	console_puts(" - vbr at ");
	console_putnum(cacr);
	putr();
#endif

  /* Later we'll decide the best place to move the kernel */
  linux_start_addr = (unsigned char *)0x04480000;
  console_puts(" - linux_start_addr at ");
  console_putnum((unsigned long)linux_start_addr);
  putr();

  console_puts(" Copying image data ...");
  dst = (unsigned char *)linux_start_addr;
  /* We don't want to mess with overflows, are we? */
  k = 0UL;
  img = &image_data[0];
  while (k < image_size) {
    *dst++ = *img++;
    k++;
  }
  console_puts(" Ok. ");
  console_puts("[");
  console_putnum10(image_size);
  console_puts(" bytes]\n");

  console_puts("\n - Registers at boot time.\n");
  register_dump();
  putr();

#if 0
	console_puts(" - Initial %sp pointed to ");
	console_putnum((unsigned long)&arg[0]);
	putr();
	pointer_dump(arg,30);
#endif

#if 0
	putr();
	console_puts(" - Dumping ptr at 0x00000000:\n");
	ultmp = (unsigned long *)0;
	pointer_dump(ultmp,100);
#endif

#if 0
	console_puts(" About to launch image...\n");
	for (i=0; i<1000000; i++) ;  /* just a delay to read screen info */
  	linux_rawbin_launch();
#endif

  /* if linux_*_launch() is uncommented,
     this code is not reachable */
  console_puts("\nNow Entering Infinte Loop ");
  console_puts("(right-command+tilde should break to monitor)\n");
  while (1)
    ;

  return 0;
}
