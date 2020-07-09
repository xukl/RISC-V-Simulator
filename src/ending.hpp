#ifndef ENDING_HPP
#define ENDING_HPP
#include <iostream>
#include <cstdlib>
#include <cstdint>
extern uint32_t reg[32];
void end_of_simulation()
{
	std::cout << (reg[10] & 0xff) << std::endl;
	exit(0);
}
#endif
