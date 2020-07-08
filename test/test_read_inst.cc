#include "read_inst.hpp"
#include <iostream>
inst_pool<10000> inst;
int main()
{
	read_inst(std::cin, inst);
	// use gdb to check the result
}
