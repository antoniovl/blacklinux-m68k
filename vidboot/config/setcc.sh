#!/bin/sh
# This will choose the appropiate CC string. In NeXT is not
# called gcc.
#BUILD=`./pvmgetarch`
BUILD=`./getarch`
if [ "$BUILD" = "LINUX" ]; then
  echo -n "gcc"
else
  echo -n "cc"
fi
