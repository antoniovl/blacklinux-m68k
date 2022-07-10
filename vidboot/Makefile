#
# Makefile for building the console debug tool.
# Comments & enhancements to antonio@naiki.com
#

# ********************************************************
# * I gues this it's the only user configurable section. *
# ********************************************************

# Set this to your cross compiler prefix, if needed.
# This must be set when building in a Linux Cross-Compiling
# environment. Not applies if you're building with NeXT tools 
# (NeXTstep), so it can be ommited.
PREFIX="m68k-linux-"

# Set the required debugging flags used with C code:
# DO_MONOCHROME_VIDEO	Build image with mono display support
# DO_COLOR_VIDEO	Build image with color display support
# DO_KEYB_TEST		Test keyboard input
# DO_SIZEMEM		Do memory sizing
# SHOW_CONSOLE_SETTINGS	Show how console is configured
# SHOW_MEMSPEED		Show memory speed
# SHOW_SCR1		Show scr1 content
# SHOW_KM_CONSOLE_INFO	Show content of mg->km_coni struct
CDEBUGFLAGS=-DDO_MONOCHROME_VIDEO -DDO_KEYB_TEST -DDO_SIZEMEM -DSHOW_MEMSPEED

# Set the required debugging flags used with asm code:
ASMDEBUGFLAGS=

# ********************************************************
# * You sould not need to change anything below this.    *
# ********************************************************
# Directory of system scripts.
CFG="config/"
# Detect Machine Architecture
BUILD=__`(cd $(CFG); ./getarch)`_BUILD

CROSS=`(cd $(CFG); ./setprefix.sh $(PREFIX))`
CC=$(CROSS)`(cd $(CFG); ./setcc.sh)`
ASM=$(CROSS)as
LD=$(CROSS)ld

INC=-I. -I./next

CFLAGS=-c -g $(INC) -D$(BUILD) $(CDEBUGFLAGS)
ASMFLAGS=-c -g -D$(BUILD) $(ASMDEBUGFLAGS)
# -Wall

# Net
LDFLAGS1=`(cd $(CFG); ./setldflags.sh net)`
# SCSI boot
LDFLAGS2=`(cd $(CFG); ./setldflags.sh sd)`
# Net & elf
LDFLAGS_NETELF=`(cd $(CFG); ./setldflags.sh net-elf)`

SIMPKERN=./simpkern/simpkern

SOURCES=main.c asm-tools.S memory.S string.c console.c debug.c image.h vbr.h
# asm-tools.o contains the startup code, so must be first in line.
# memory.S contains sensitive code, must be second.
OBJS=asm-tools.o memory.o main.o main_setup.o string.o console.o debug.o
BOOTFILE=vid.boot

default:
	@more build.txt


#********************************
# Ready for module incorporation
#********************************
# - Netbooted preload exe
net: $(OBJS) $(SIMPKERN)
	$(WHEREAMI)
	$(LD) $(OBJS) -o $(BOOTFILE) $(LDFLAGS1)
	(cd $(CFG); ./build-net-img.sh ../$(SIMPKERN) ../$(BOOTFILE))

# - Executable intended to be booted from sd
# sd: $(OBJS) $(SIMPKERN)
#	$(WHEREAMI)
#	$(LD) $(OBJS) -o $(BOOTFILE) $(LDFLAGS2)

# Builds a file ready to be dumped to a diskette to buid a 
# boot floppy. 
floppy: $(OBJS) $(SIMPKERN)
	$(WHEREAMI)
	$(LD) $(OBJS) -o $(BOOTFILE) $(LDFLAGS1)
	(cd $(CFG); ./build-flp-img.sh ../$(SIMPKERN) ../$(BOOTFILE))

# net-elf it's for debuggin purposes. It will build an image
# able to be disassembled by m68k-linux-objdump, when working
# with Linux.
net-elf: $(OBJS)
	$(LD) $(OBJS) -o $(BOOTFILE) $(LDFLAGS_NETELF)

# Object file building
main.o: main.c main_setup.h next/next-mon-global.h image.h vbr.h
	$(CC) main.c -o main.o $(CFLAGS)

asm-tools.o: asm-tools.S asm-tools.h
	$(CC) asm-tools.S -o asm-tools.o $(ASMFLAGS)

memory.o: memory.S memory.h asm-setup.h vbr.h
	$(CC) memory.S -o memory.o $(ASMFLAGS)

string.o: string.c string.h
	$(CC) string.c -o string.o $(CFLAGS)

console.o: console.h console.c
	$(CC) console.c -o console.o $(CFLAGS)

main_setup.o: main_setup.c main_setup.h
	$(CC) main_setup.c -o main_setup.o $(CFLAGS)

debug.o: debug.c debug.h
	$(CC) debug.c -o debug.o $(CFLAGS)

$(SIMPKERN):
	(cd simpkern; make)

clean:
	rm -rf *.o $(BOOTFILE) $(SIMPKERN) config/tmpimg





