#DEFS += -DUSE_GPU
#DEFS += -DDEBUG

PBP = EBOOT.PBP
BINARY = out
OBJS = startup.o gbcore/cpu.o gbcore/gb.o gbcore/lcd.o gbcore/sgb.o \
	gbcore/rom.o gbcore/mbc.o gbcore/apu.o gbcore/cheat.o \
	main.o pg.o renderer.o menu.o filer.o sound.o saveload.o image.o gz.o \
	syscall.o
LIBS = lib/unziplib.a lib/libpng.a lib/libz.a -lc

all: $(PBP)

$(PBP): $(BINARY)
	outpatch
	elf2pbp outp "RIN GB/GBC Emulator"

$(BINARY): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $@
	$(STRIP) $(BINARY)

%.o: %.c
	$(CC) $(DEFS) $(INCLUDES) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(DEFS) $(ARCHFLAGS) -c $< -o $@

clean:
	$(RM) *.o *.map out outp gbcore/*.o

include Makefile.psp
