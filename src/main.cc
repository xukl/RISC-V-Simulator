#include <iostream>
#ifdef DEBUG
#include "debug.hpp"
#endif
extern void IF();
extern void ID();
extern void EX();
extern void MEM();
extern void WB();
extern void read_inst(std::istream &);
int main()
{
	std::ios::sync_with_stdio(false);
	read_inst(std::cin);
	while (true)
	{
		IF();
		ID();
		EX();
		MEM();
		WB();
#ifdef DEBUG
		print_ID_result();
		print_reg();
		print_stall();
#endif
	}
}
