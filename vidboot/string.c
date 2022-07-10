/*
 * String handling routines impementation.
 */
#include "string.h"

unsigned strlen(char *str) {
  unsigned i, l;
  i = 0;
  l = 0;
  /* Does not count the NULL */
  while (str[i++] != 0)
    l++;
  return (l);
}

unsigned strcat(char *dst, const char *src) {
  int top;

  top = strlen(dst);
  strcpy(&dst[top], src);
  return 0;
}

unsigned strcpy(char *dst, const char *src) {
  unsigned i;
  /* The must be enough space in dst to hold src */
  i = 0;
  while (src[i] != 0) {
    dst[i] = src[i];
    i++;
  }
  dst[i] = 0;
  return (i);
}

unsigned strncpy(char *dst, const char *src, unsigned int nbytes) {
  unsigned i;
  i = 0;
  while ((src[i] != 0) && (i < nbytes)) {
    dst[i] = src[i];
    i++;
  }
  return (i);
}

unsigned bcopy(void *src, void *dst, unsigned nbytes) {
  unsigned i;
  void *src1, *dst1;
  src1 = src; /* let's keep src & dst untouched */
  dst1 = dst;
  /* keeping i as unsigned, the maximum chunk of data copied
     will be 4Gb */
  for (i = 0; i < nbytes; i++) {
    *((char *)dst1) = *((char *)src1);
    dst1 += sizeof(char);
    src1 += sizeof(char);
  }
  return (i);
}

void bzero(void *s, int n) {
  register unsigned i;
  for (i = 0; i < n; i++) {
    *(char *)s = (char)0;
  }
  return;
}