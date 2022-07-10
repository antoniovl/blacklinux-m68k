/*
 * Configuration of the type of machine.
 */

#include "main_setup.h"
#include "string.h"

char machine_string[255];
struct machine_config NeXT_cfg;

/* Varables loaded at boot time */
unsigned long reg_d0, reg_d1, reg_d2, reg_d3, reg_d4, reg_d5, reg_d6, reg_d7,
    *reg_a0, *reg_a1, *reg_a2, *reg_a3, *reg_a4, *reg_a5, *reg_a6, *reg_a7;

char *select_mtype(int machine_type) {
  char *c;
  switch (machine_type) {
  case NeXT_CUBE:
    c = "NeXTCube ";
    break;
  case NeXT_WARP9:
    c = "NeXTStation ";
    break;
  case NeXT_X15:
    c = "NeXTCube ";
    break;
  case NeXT_WARP9C:
    c = "NeXTStation ";
    break;
  default:
    c = "(Don't know yet!) ";
    break;
  }
  return (c);
}

/*
 * Configures according to current system.
 * Testing a heuristic method to figure out
 * system type, just specify video (MONO/COLOR).
 */
int machine_setup(int video) {
  int machine;
  char *tmpstr;

  /* Get the CPU type */
  NeXT_cfg.cpu_type = check_cpu();

  /* Get the slot_id value */
  /* Be careful to avoid changing reg_d0 before this! */
  /* 042698: Guess based on register dump */
  /* 062498: Takes slot_id from mon_global struct */
  /* NeXT_cfg.slot_id = (unsigned)reg_d0; */
  NeXT_cfg.slot_id = mg->mg_sid;
  /* NeXT_cfg.slot_id = 0; */
  NeXT_cfg.slot_id_bmap = (NeXT_cfg.cpu_type == M68040) ? 0x00100000 : 0x0;

  /* Get the scr1 */
  NeXT_cfg.scr1 = *((struct scr1 *)(NeXT_cfg.slot_id + SCR1_ADDR));
  /* Can be posible that instead of == goes a >= ? */
  if (scr1_board_revision(NeXT_cfg.scr1.cpu_rev) == 0x4)
    NeXT_cfg.scr1 = *((struct scr1 *)(NeXT_cfg.slot_id + 0x2200000));
  NeXT_cfg.dma_rev = NeXT_cfg.scr1.dma_rev;
  NeXT_cfg.cpu_rev = NeXT_cfg.scr1.cpu_rev;
  NeXT_cfg.machine_type = scr1_machine_type(NeXT_cfg.scr1.cpu_rev);
  NeXT_cfg.board_revision = scr1_board_revision(NeXT_cfg.scr1.cpu_rev);
  NeXT_cfg.videomem_speed = NeXT_cfg.scr1.videomem_speed;
  NeXT_cfg.mainmem_speed = NeXT_cfg.scr1.mainmem_speed;
  NeXT_cfg.cpu_clock = NeXT_cfg.scr1.cpu_clock;

  /* If the cpu_type is 030, this is Non-Turbo */
  if (NeXT_cfg.cpu_type == M68030) {
    machine = (video == VIDEO_MONO) ? MACHINE_NTM : MACHINE_NTC;
  } else {
    /* If speed it's greater than 25Mhz, then it's a turbo.
       Maybe we should be based on the DMA rev. */
    switch (NeXT_cfg.cpu_clock) {
    case CPU_33MHZ:
    case CPU_40MHZ:
      machine = (video == VIDEO_MONO) ? MACHINE_TM : MACHINE_TC;
      break;
    default:
      machine = (video == VIDEO_MONO) ? MACHINE_NTM : MACHINE_NTC;
    }
  }

  switch (machine) {
  case MACHINE_NTM:
    BI_videoaddr = NTM_VIDEO;
    BI_videorow = NTM_VIDEOW / 4;
    BI_dimensions = (NTM_VIDEOH << 16) + NTM_VIDEOW;
    BI_videoheight = NTM_VIDEOH;
    BI_videowidth = NTM_VIDEOW;
    BI_nppw = NTM_NPPW;       /* 16 */
    BI_nbpp = (32 / BI_nppw); /* 2 */
    BI_colormask = 0x3;
    BI_white = NTM_WHITE;
    BI_lt_gray = NTM_LT_GRAY;
    BI_dk_gray = NTM_DK_GRAY;
    BI_black = NTM_BLACK;
    tmpstr = select_mtype(NeXT_cfg.machine_type);
    strcpy(machine_string, tmpstr);
    strcat(machine_string, "Mono.");
    break;
  case MACHINE_NTC:
    BI_videoaddr = NTC_VIDEO;
    BI_videorow = NTC_VIDEOW * 2;
    BI_dimensions = (NTC_VIDEOH << 16) + NTC_VIDEOW;
    BI_videoheight = NTC_VIDEOH;
    BI_videowidth = NTC_VIDEOW;
    BI_nppw = NTC_NPPW;       /* 2 */
    BI_nbpp = (32 / BI_nppw); /* 16 */
    BI_colormask = 0xffff;
    BI_white = NTC_WHITE;
    BI_lt_gray = NTC_LT_GRAY;
    BI_dk_gray = NTC_DK_GRAY;
    BI_black = NTC_BLACK;
    tmpstr = select_mtype(NeXT_cfg.machine_type);
    strcpy(machine_string, tmpstr);
    strcat(machine_string, "Color-16bpp.");
    break;
  case MACHINE_TM:
    BI_videoaddr = TM_VIDEO;
    BI_videorow = TM_VIDEOW / 4;
    BI_dimensions = (TM_VIDEOH << 16) + TM_VIDEOW;
    BI_videoheight = TM_VIDEOH;
    BI_videowidth = TM_VIDEOW;
    BI_nppw = TM_NPPW;        /* 16 */
    BI_nbpp = (32 / BI_nppw); /* 2 */
    BI_colormask = 0x3;
    BI_white = TM_WHITE;
    BI_lt_gray = TM_LT_GRAY;
    BI_dk_gray = TM_DK_GRAY;
    BI_black = TM_BLACK;
    tmpstr = select_mtype(NeXT_cfg.machine_type);
    strcpy(machine_string, tmpstr);
    strcat(machine_string, "Turbo Mono.");
    break;
  case MACHINE_TC:
    BI_videoaddr = TC_VIDEO;
    BI_videorow = TC_VIDEOW * 2;
    BI_dimensions = (TC_VIDEOH << 16) + TC_VIDEOW;
    BI_videoheight = TC_VIDEOH;
    BI_videowidth = TC_VIDEOW;
    BI_nppw = TC_NPPW;
    BI_nbpp = (32 / BI_nppw);
    BI_colormask = 0xffff;
    BI_white = TC_WHITE;
    BI_lt_gray = TC_LT_GRAY;
    BI_dk_gray = TC_DK_GRAY;
    BI_black = TC_BLACK;
    tmpstr = select_mtype(NeXT_cfg.machine_type);
    strcpy(machine_string, tmpstr);
    strcat(machine_string, "Turbo Color-16bpp.");
    break;
  case MACHINE_CDB:
    BI_videoaddr = CDB_VIDEO;
    BI_videorow = CDB_VIDEOW * 2;
    BI_dimensions = (CDB_VIDEOH << 16) + CDB_VIDEOW;
    BI_videoheight = CDB_VIDEOH;
    BI_videowidth = CDB_VIDEOW;
    BI_nppw = CDB_NPPW;
    BI_nbpp = (32 / BI_nppw);
    BI_colormask = 0xffffffff;
    BI_white = CDB_WHITE;
    BI_lt_gray = CDB_LT_GRAY;
    BI_dk_gray = CDB_DK_GRAY;
    BI_black = CDB_BLACK;
    strcpy(machine_string, CDB_MACHINE_STRING);
    break;
  default:
    break;
  }

  return 0;
}
