#include <iostream>
#include <cstdlib>
#include <cstdint>
extern const volatile uint32_t reg[32];
void end_of_simulation()
{
	std::cout << std::dec << (reg[10] & 0xff) << std::endl;
	exit(0);
}
