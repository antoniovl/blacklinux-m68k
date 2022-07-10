#if !defined(__VBR_H__)

struct scb {
  int vbr_issp;            /* 00 - initial ssp */
  int (*vbr_ipc)();        /* 04 - initial pc */
  int (*vbr_buserr)();     /* 08 - bus error */
  int (*vbr_addrerr)();    /* 0c - address error */
  int (*vbr_illegal)();    /* 10 - illegal instruction */
  int (*vbr_zerodiv)();    /* 14 - zero divide */
  int (*vbr_check)();      /* 18 - CHK, CHK2 instruction */
  int (*vbr_trapv)();      /* 1c - TRAPV, cpTRAPcc, TRAPcc inst */
  int (*vbr_privilege)();  /* 20 - privilege violation */
  int (*vbr_trace)();      /* 24 - trace trap */
  int (*vbr_emu1010)();    /* 28 - 1010 emulator trap */
  int (*vbr_emu1111)();    /* 2c - 1111 emulator trap */
  int (*vbr_stray1)();     /* 30 - reserved */
  int (*vbr_coproc)();     /* 34 - coprocessor protocol error */
  int (*vbr_format)();     /* 38 - stack format error */
  int (*vbr_badintr)();    /* 3c - uninitialized interrupt */
  int (*vbr_stray2[8])();  /* 40-5c - reserved */
  int (*vbr_spurious)();   /* 60 - spurious interrupt */
  int (*vbr_ipl[7])();     /* 64-7c - ipl 1-7 autovectors */
  int (*vbr_trap[16])();   /* 80-bc - trap instruction vectors */
  int (*vbr_stray3[16])(); /* c0-fc - reserved */
  int (*vbr_user[192])();  /* 100-3fc - user interrupt vectors */
};

#define __VBR_H__
#endif