#include <iostream>
#include "read_inst.hpp"
#include "debug.hpp"
#include <unistd.h>
extern void IF();
extern void ID();
extern void EX();
extern void MEM();
extern void WB();
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
		print_ID_result();
		print_reg();
		print_stall();
		sleep(1);
	}
}
