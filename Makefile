#DEFS += -DUSE_GPU
#DEFS += -DDEBUG
MODE=REL
#MODE=DBG

TARGET = rin_rm
OBJS = gbcore/cpu.o gbcore/gb.o gbcore/lcd.o gbcore/sgb.o \
	gbcore/rom.o gbcore/mbc.o gbcore/apu.o gbcore/cheat.o \
	main.o pg.o renderer.o rewind.o menu.o filer.o sound.o saveload.o image.o gz.o menu_rewind.o menu_credits.o tools.o

INCDIR		=

ARCHFLAGS =-mgp32 -mlong32 -msingle-float -mabi=eabi
CFLAGS = -Wall -fomit-frame-pointer $(ARCHFLAGS) -G3 -Wdouble-promotion

ifeq ($(MODE),REL)
CFLAGS += -O3
else
CFLAGS += -O0 -ggdb -gdwarf-2
endif

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS	= $(CFLAGS)

LIBDIR	=
LDFLAGS	= 
LIBS = lib/unziplib.a lib/libpng.a -lz -lc -lm -lpspnet_inet -lpspuser -lpsppower -lpspaudio -lpsprtc

BUILD_PRX = 0

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_TITLE = Rin 1.32 RM v2

PSPSDK	= $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

DST = /Users/maciek/git/psp_rin_release/RIN_1_32_RM
SRC = ${TARGET}.prx
install :
	rm -f ${DST}/${SRC}
	rm -f ${DST}/*.elf
	#cp -f *.prx ${DST}
	cp -f *.elf ${DST}
	#chmod 777 ${DST}/*.prx
	chmod 777 ${DST}/*.elf
