#DEFS += -DUSE_GPU
#DEFS += -DDEBUG
TARGET		= hello_world
OBJS = gbcore/cpu.o gbcore/gb.o gbcore/lcd.o gbcore/sgb.o \
	gbcore/rom.o gbcore/mbc.o gbcore/apu.o gbcore/cheat.o \
	main.o pg.o renderer.o rewind.o menu.o filer.o sound.o saveload.o image.o gz.o \
	syscall.o


INCDIR		=

ARCHFLAGS=-mgp32 -mlong32 -msingle-float -mabi=eabi
CFLAGS= -Wall -O3 -fomit-frame-pointer $(ARCHFLAGS)
CXXFLAGS	= $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS	= $(CFLAGS)

LIBDIR		=
LDFLAGS	=
LIBS = lib/unziplib.a lib/libpng.a lib/libz.a -lc -lpspnet_inet -lpspuser -lpsppower -lpspaudio -lpsprtc

BUILD_PRX = 0

EXTRA_TARGETS	= EBOOT.PBP
PSP_EBOOT_TITLE= Hello World

PSPSDK	= $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
