#!/bin/sh
# This program will echo the needed prefix if cross-compiling.
# Done this way because the lack of "if" statements in NeXT's make.
# If no prefix it's specified, it will default to "m68k-linux-" when called
# in Linux

if [ $# -gt 1 ]; then
  echo "setprefix: Wrong Args."
  exit 1
fi

#BUILD=`./pvmgetarch`
BUILD=`./getarch`

if [ $# -eq 1 ]; then
  PREFIX="$1"
else
  PREFIX="m68k-linux-"
fi

if [ "$BUILD" = "LINUX" ]; then
  echo "$PREFIX"
else
  echo -n ""
fi
