CXXFLAGS=-Wall -Wextra -std=c++20 -ggdb
LDFLAGS=-lstdc++

.PHONY: all clean
all: main
main: ending.o EX.o ID.o IF.o main.o MEM.o read_inst.o state.o WB.o
ending.o: ending.cc state.hpp inst.hpp
EX.o: EX.cc inst.hpp state.hpp
ID.o: ID.cc inst.hpp state.hpp
IF.o: IF.cc inst.hpp state.hpp
main.o: main.cc state.hpp inst.hpp debug.hpp
MEM.o: MEM.cc inst.hpp state.hpp
read_inst.o: read_inst.cc
state.o: state.cc state.hpp inst.hpp
WB.o: WB.cc inst.hpp state.hpp
clean:
	rm *.o
