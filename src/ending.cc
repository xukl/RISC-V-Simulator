#include <iostream>
#include <cstdlib>
#include <cstdint>
#include "state.hpp"
extern const volatile state old_state;
const volatile uint32_t *const reg = old_state.reg;
void end_of_simulation()
{
	std::cout << std::dec << (reg[10] & 0xff) << std::endl;
	exit(0);
}
