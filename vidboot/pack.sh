#!/bin/sh
# This must be run from the vidboot dir.

if [ $# -eq 0 ]; then
  echo "Usage: $0 <date-string> [-z|Z|b]"
  echo "   date-string: Date-R id string, ejm. 042198-1 (no quotes)"
  echo "   z: Compress using \"gzip -9\"".
  echo "   Z: Compress with standard \"compress\""
  echo "   b: Compress with \"bzip2\"".
  exit 1
fi

# Try to figure out if really are there
if [ ! -f ./asm-setup.h ]; then
  echo "$0: Maybe we're not in the righ place."
  exit 1
fi

# Warning: All this files are relative to vidboot dir.
D="vidboot-$1"
TAR="vidboot-$1.tar"
NEXT="$D/next/*"
CFG="$D/config/*"
HGEN="$D/hgen/*.[chS] $D/hgen/Make*"
SIMPKERN="$D/simpkern/*.[chS] $D/simpkern/Make*"
MAIN="$D/*.[chS] $D/Make* $D/*.txt $D/pack.sh"
BIN_IMGS="$D/vid.boot.*"
TARFILES="$MAIN $NEXT $CFG $HGEN $SIMPKERN $BIN_IMGS"

# Already exist?
if [ -f "$TAR" -o -f "$TAR.gz" -o -f "$TAR.Z" -o -f "$TAR.bz2" -o \
     -f "old/$TAR" -o -f "old/$TAR.gz" -o -f "old/$TAR.Z" ]; then
  echo "pack.sh: one of"
  echo "  - $TAR"
  echo "  - $TAR.gz"
  echo "  - $TAR.Z" 
  echo "  - $TAR.bz2"
  echo "already exists."
  exit 1
fi

# Will gzip?
if [ $# -eq 2 ]; then
  ZIP="no"
  if [ "$2" = "-z" ]; then
    ZIP="gzip"
  elif [ "$2" = "-Z" ]; then
    ZIP="comp"
  elif [ "$2" = "-b" ]; then
    ZIP="bz2"
  else
    echo "$0: Unkown option $2."
    exit 1
  fi
fi

cd ..
echo "Creating Directory Link vidboot-$1 -> vidboot"
ln -s vidboot vidboot-$1
tar -cvf $TAR $TARFILES
mv $TAR $D

# Do compressing if requested
if [ $# -eq 2 ]; then  
	case $ZIP in
	gzip) echo -n "gzip: "
	      gzip -9 -v $D/$TAR ;;
	comp) echo -n "compress: "
	      compress $D/$TAR ;;
	bz2)  echo -n "bzip2: "
	      bzip2 -v $D/$TAR ;;
	esac
fi

rm vidboot-$1