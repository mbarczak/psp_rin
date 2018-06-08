STATICLIB = libz.a

#LOC = -DASMV
#LOC = -DDEBUG -g

CC = psp-gcc
ARCHFLAGS=-mgp32 -mlong32 -msingle-float -mabi=eabi 
CFLAGS = $(LOC) -O3 -Wall $(ARCHFLAGS)

AR = psp-ar
ARFLAGS = rcs

RM = rm -f

OBJS = adler32.o compress.o crc32.o deflate.o gzio.o infback.o \
       inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o

all: $(STATICLIB)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(STATICLIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	-$(RM) $(STATICLIB)
	-$(RM) *.o

adler32.o: zlib.h zconf.h
compress.o: zlib.h zconf.h
crc32.o: crc32.h zlib.h zconf.h
deflate.o: deflate.h zutil.h zlib.h zconf.h
example.o: zlib.h zconf.h
gzio.o: zutil.h zlib.h zconf.h
inffast.o: zutil.h zlib.h zconf.h inftrees.h inflate.h inffast.h
inflate.o: zutil.h zlib.h zconf.h inftrees.h inflate.h inffast.h
infback.o: zutil.h zlib.h zconf.h inftrees.h inflate.h inffast.h
inftrees.o: zutil.h zlib.h zconf.h inftrees.h
minigzip.o: zlib.h zconf.h
trees.o: deflate.h zutil.h zlib.h zconf.h trees.h
uncompr.o: zlib.h zconf.h
zutil.o: zutil.h zlib.h zconf.h
