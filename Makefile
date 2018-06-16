#DEFS += -DUSE_GPU
#DEFS += -DDEBUG
TARGET = rin_rm
OBJS = gbcore/cpu.o gbcore/gb.o gbcore/lcd.o gbcore/sgb.o \
	gbcore/rom.o gbcore/mbc.o gbcore/apu.o gbcore/cheat.o \
	main.o pg.o renderer.o rewind.o menu.o filer.o sound.o saveload.o image.o gz.o

INCDIR		=

ARCHFLAGS =-mgp32 -mlong32 -msingle-float -mabi=eabi
CFLAGS = -Wall -O3 -fomit-frame-pointer $(ARCHFLAGS) -G3
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS	= $(CFLAGS)

LIBDIR		=
LDFLAGS	=
LIBS = lib/unziplib.a lib/libpng.a -lz -lc -lm -lpspnet_inet -lpspuser -lpsppower -lpspaudio -lpsprtc

BUILD_PRX = 0

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_TITLE = Rin 1.32 RM v2

PSPSDK	= $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

DST = /dst
SRC = ${TARGET}.prx
install :
	rm -f ${DST}/${SRC}
	cp *.prx ${DST}
	chmod 777 ${DST}/*.prx
