#include <iostream>
#include <fstream>
#include <cstring>
#include "state.hpp"
//#define DEBUG
#ifdef DEBUG
#include "debug.hpp"
#endif
extern void IF();
extern void ID();
extern void EX();
extern void MEM();
extern void WB();
extern void read_inst(std::istream &);
extern state old_state, new_state;
int main(int argc, char **argv)
{
	if (argc >= 2)
	{
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
		{
			std::cout << "Usage: {PROGRAM} [-f data_file]\n"
				"    A simple RISC-V CPU simulator.\n"
				"data_file: the program to run.\n"
				"    with no \"-f data_file\", read standard input.\n";
			return 0;
		}
		if (argc >= 3 && strcmp(argv[1], "-f") == 0)
		{
			std::ifstream fin(argv[2]);
			if (!fin)
			{
				std::cerr << "Cannot open file " << argv[2] << ". Abort.\n";
				return 1;
			}
			read_inst(fin);
		}
	}
	else
	{
		std::ios::sync_with_stdio(false);
		read_inst(std::cin);
	}
	while (true)
	{
		IF();
		ID();
		EX();
		MEM();
		WB();
#ifdef DEBUG
		print_reg();
		print_pipeline();
#endif
		old_state = new_state;
	}
}
