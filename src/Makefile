CXXFLAGS=-ggdb -Wall -Wextra -std=c++20 -I../src
LDFLAGS=-lstdc++

.PHONY: all clean
all: main
main: main.o IF.o ID.o EX.o MEM.o WB.o state.o ending.o read_inst.o
ending.o: inst.hpp
read_inst.o: inst.hpp
IF.o: inst.hpp
ID.o: inst.hpp
EX.o: inst.hpp
MEM.o: inst.hpp
WB.o: inst.hpp
clean:
	rm *.o
