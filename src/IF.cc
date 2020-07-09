#include <cstdint>
extern uint32_t pc;
extern uint8_t memory[];

extern uint32_t IF_result;
void IF()
{
	IF_result = *reinterpret_cast<uint32_t*>(memory + pc);
}
