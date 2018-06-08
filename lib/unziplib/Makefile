LIB = unziplib.a
OBJECTS = unzip.o ioapi.o zlibInterface.o

all: $(LIB)

$(LIB): $(OBJECTS)
	psp-ar rcs $@ $(OBJECTS)

%.o : %.c
	psp-gcc -O3 -fomit-frame-pointer -mgp32 -mlong32 -mabi=eabi -c $< -o $@

clean:
	del /s /f *.o *.map
