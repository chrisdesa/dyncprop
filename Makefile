CFLAGS := -std=gnu99
CPPFLAGS := -I ./src
LFLAGS := -march=i386 -m32

INSTR_CPPS := $(wildcard src/instr/*.cpp)
INSTR_HPPS := $(wildcard src/instr/*.hpp)
INSTR_OBJS := $(patsubst src/instr/%.cpp,obj/instr/%.o,$(INSTR_CPPS))

CLASS_HPPS := src/Instr.hpp src/Home.hpp src/Data.hpp src/State.hpp
CLASS_OBJS := obj/Instr.o obj/Home.o obj/Data.o obj/State.o
OBJS := obj/main.o obj/dyncprop.o $(CLASS_OBJS) $(INSTR_OBJS)

all: bin obj obj/instr bin/main

bin:
	mkdir -p bin

obj:
	mkdir -p obj
	
obj/instr: obj
	mkdir -p obj/instr

clean:
	rm -f obj/instr/*.o obj/*.o bin/main

bin/main: $(OBJS)
	g++ $(LFLAGS) $^ -o $@

obj/main.o: test/main.c src/dyncprop.h
	gcc -c $(CFLAGS)  -I ./src/ $(LFLAGS) $< -o $@

obj/dyncprop.o: src/dyncprop.cpp src/dyncprop.h $(CLASS_HPPS)
	g++ -c $(CPPFLAGS) $(LFLAGS) $< -o $@

obj/Instr.o: src/Instr.cpp $(CLASS_HPPS) $(INSTR_HPPS)
	g++ -c $(CPPFLAGS) $(LFLAGS) $< -o $@

obj/Home.o: src/Home.cpp $(CLASS_HPPS)
	g++ -c $(CPPFLAGS) $(LFLAGS) $< -o $@

obj/Data.o: src/Data.cpp $(CLASS_HPPS)
	g++ -c $(CPPFLAGS) $(LFLAGS) $< -o $@

obj/State.o: src/State.cpp $(CLASS_HPPS)
	g++ -c $(CPPFLAGS) $(LFLAGS) $< -o $@

obj/instr/%.o: src/instr/%.cpp src/instr/%.hpp $(CLASS_HPPS)
	g++ -c $(CPPFLAGS) $(LFLAGS) $< -o $@
