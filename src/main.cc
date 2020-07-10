#include <iostream>
//#define DEBUG
#ifdef DEBUG
#include "debug.hpp"
#include <fstream>
#include <cassert>
#endif
extern void IF();
extern void ID();
extern void EX();
extern void MEM();
extern void WB();
extern void read_inst(std::istream &);
int main(int argc, char **argv)
{
	std::ios::sync_with_stdio(false);
#ifdef DEBUG
	assert(argc > 1);
	std::ifstream fin(argv[1]);
	read_inst(fin);
#else
	read_inst(std::cin);
#endif
	while (true)
	{
		WB();
		MEM();
		EX();
		ID();
		IF();
#ifdef DEBUG
		print_reg();
		print_pipeline();
#endif
	}
}
