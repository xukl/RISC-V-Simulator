#include <cstdint>
#include "state.hpp"

void IF()
{
	IF_result = *reinterpret_cast<uint32_t*>(pc);
	pc += 4;
}
