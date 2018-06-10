#DEFS += -DUSE_GPU
#DEFS += -DDEBUG

PSPSDK=$(shell psp-config --pspsdk-path)

PSPDEV=$(shell psp-config --pspdev-path)/bin
INCLUDES=$(PSPSDK)/include


PBP = EBOOT.PBP
BINARY = out
OBJS = startup.o gbcore/cpu.o gbcore/gb.o gbcore/lcd.o gbcore/sgb.o \
	gbcore/rom.o gbcore/mbc.o gbcore/apu.o gbcore/cheat.o \
	main.o pg.o renderer.o rewind.o menu.o filer.o sound.o saveload.o image.o gz.o \
	syscall.o
LIBS = -L/pspdev/psp/sdk/lib lib/unziplib.a lib/libpng.a lib/libz.a -lc -lpspnet_inet -lpspuser

all: $(PBP)

$(PBP): $(BINARY)
	#outpatch
	psp-fixup-imports -o outp out
	#elf2pbp out "RIN GB/GBC Emulator"
	mksfo "Rin 1.32 RM" param.sfo
	pack-pbp eboot.pbp param.sfo ICON0.PNG NULL NULL NULL NULL outp NULL

$(BINARY): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $@
	$(STRIP) $(BINARY)

%.o: %.c
	$(CC) $(DEFS) $(INCLUDES) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(DEFS) $(ARCHFLAGS) -c $< -o $@

clean:
	$(RM) *.o *.map out outp param.sfo gbcore/*.o

include Makefile.psp
