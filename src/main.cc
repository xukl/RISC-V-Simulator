#include <iostream>
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
		print_reg();
		print_pipeline();
#endif
		old_state = new_state;
	}
}
