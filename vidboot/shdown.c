#include <stdio.h>
#include <stdlib.h>

/*
 * I'm too lazy for doing a "su" then a shutdown every 5 minutes to
 * test a new program. Better I have this setuid(0).
 * Please notice that anyone else uses my machine, so having this
 * monster doesn't hurt.
 */

int main() {
  setuid(0);
  system("shutdown -r now");
  return 0;
}