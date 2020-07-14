CXXFLAGS=-Wall -Wextra -std=c++20 -I../src -O2
LDFLAGS=-lstdc++

.PHONY: all clean
all: main
main: main.o IF.o ID.o EX.o MEM.o WB.o state.o ending.o read_inst.o state.o
ending.o: inst.hpp state.hpp
read_inst.o: inst.hpp state.hpp
IF.o: inst.hpp state.hpp
ID.o: inst.hpp state.hpp
EX.o: inst.hpp state.hpp
MEM.o: inst.hpp state.hpp
WB.o: inst.hpp state.hpp
clean:
	rm *.o
