
LFLAGS := -march=i386 -m32

all: bin obj bin/main

bin:
	mkdir bin

obj:
	mkdir obj

bin/main: obj/main.o obj/x86emu.o obj/x86opcodes.o
	gcc $(LFLAGS) $^ -o $@

obj/main.o: src/main.c src/x86emu.h
	gcc -c $(LFLAGS) $< -o $@

obj/x86emu.o: src/x86emu.c src/x86emu.h
	gcc -c $(LFLAGS) $< -o $@

obj/x86opcodes.o: src/x86opcodes.c src/x86emu.h
	gcc -c $(LFLAGS) $< -o $@
