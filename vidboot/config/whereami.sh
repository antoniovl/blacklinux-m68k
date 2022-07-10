#!/bin/sh

echo "*************************************"

BUILD=`./getarch`
case $BUILD in
	NeXT)  echo "**** Building for a NeXT System  ****" ;;
	LINUX) echo "**** Building for a Linux System ****" ;;
esac

echo "*************************************"

