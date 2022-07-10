/*
 * Some data structures needed by memory.S
 */

#if !defined(__MEMORY_H__)

struct memory_elem {
  void *addr;
  unsigned long size;
};

struct memory_list {
  unsigned long count;
  struct memory_elem mem[8];
};

#define __MEMORY_H__
#endif