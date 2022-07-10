#!/bin/sh
# Assumes that it's running from config/ dir.
BUILD=`./getarch`
if [ "$BUILD" = "LINUX" ]; then
  echo ""
  echo "build-img: Building Net Bootable from $BUILD Arch: "
  $1 $2 tmpimg
  cat tmpimg > $2
  echo "build-img: Done."
  echo ""
else
  echo ""
  echo "build-img: Nothing to do for $BUILD Arch (not needed). "
  echo "build-img: Leaving Image untouched."
  echo ""
fi
