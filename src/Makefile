CXXFLAGS=-ggdb -Wall -Wextra -std=c++20 -I../src
LDFLAGS=-lstdc++

.PHONY: all clean
all: main
main: main.o IF.o ID.o EX.o MEM.o WB.o state.o
clean:
	rm *.o
