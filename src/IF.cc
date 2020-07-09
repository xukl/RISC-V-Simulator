#include <cstdint>
#include "inst.hpp"
extern const volatile uint32_t pc;
extern const volatile uint8_t memory[];

extern uint32_t IF_result;
static bool end_reached = false;
void IF()
{
	if (end_reached)
		IF_result = IF_NOP;
	else
	{
		IF_result = *reinterpret_cast<const volatile uint32_t*>(memory + pc);
		if (IF_result == 0x0ff00513)
			end_reached = true;
	}
}
