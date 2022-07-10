#!/bin/sh
# Usage:
#   setldflags.sh <net|sd|net-elf>
# It will determine the best ldflags string 
# based on pvmgetarch result.

if [ $# -gt 1 ]; then
  echo "setldflags: Usage: setldflags.sh <net|sd|net-elf>"
  echo "net-elf only will allow easy dissasembly in Linux."
  exit 1
fi

ADDR_NET=0x04380000
ADDR_SD=0x04000000

# It seems that -nostdlib is an option to cc but not to ld.
# NEXT_COMMON="-preload -nostdlib"
# NEXT_COMMON="-preload -segalign 4 "
NEXT_COMMON="-preload "
LINUX_COMMON="-oformat binary -e _start"
LINUX_NETELF="-oformat elf32-m68k -e _start"

#BUILD="`./pvmgetarch`-$1"
BUILD="`./getarch`-$1"

case "$BUILD" in
  NeXT-net)	echo "$NEXT_COMMON -segaddr __TEXT $ADDR_NET" ;;
  NeXT-sd)	echo "$NEXT_COMMON -segaddr __TEXT $ADDR_SD" ;;
  NeXT-net-elf)	echo "Invalid Option \"elf\" to NeXT Linker"
		exit 1;;
  LINUX-net)	echo "$LINUX_COMMON -Ttext $ADDR_NET" ;;
  LINUX-sd)	echo "$LINUX_COMMON -Ttext $ADDR_SD" ;;
  LINUX-net-elf)
		echo "$LINUX_NETELF -Ttext $ADDR_NET" ;;
  *)		echo "what?"
esac

