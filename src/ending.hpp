#ifndef ENDING_HPP
#define ENDING_HPP
#include <iostream>
#include <cstdlib>
#include "state.hpp"
void end_of_simulation()
{
	std::cout << reg[10] << std::endl;
	exit(0);
}
#endif
