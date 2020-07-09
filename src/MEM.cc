#include <cstdint>
#include "state.hpp"

static int MEM_cycle = 0;
void MEM()
{
	auto &val = MEM_result.val;
	if (MEM_cycle == 2)
	{
		switch (EX_result.opcode)
		{
			case opcode::LOAD:
				switch (EX_result.s_l_info)
				{
					case -1:
						val = *reinterpret_cast<int8_t*>(memory + EX_result.val);
						break;
					case 1:
						val = *reinterpret_cast<uint8_t*>(memory + EX_result.val);
						break;
					case -2:
						val = *reinterpret_cast<int16_t*>(memory + EX_result.val);
						break;
					case 2:
						val = *reinterpret_cast<uint16_t*>(memory + EX_result.val);
						break;
					case -4:
						val = *reinterpret_cast<int32_t*>(memory + EX_result.val);
						break;
				}
				break;
			case opcode::STORE:
				switch (EX_result.s_l_info)
				{
					case -1:
						*reinterpret_cast<int8_t*>(memory + EX_result.val) = reg[EX_result.reg];
						break;
					case -2:
						*reinterpret_cast<int16_t*>(memory + EX_result.val) = reg[EX_result.reg];
						break;
					case -4:
						*reinterpret_cast<int32_t*>(memory + EX_result.val) = reg[EX_result.reg];
						break;
				}
				break;
		}
		MEM_cycle = 0;
		MEM_result.opcode = EX_result.opcode;
		MEM_result.reg = EX_result.reg;
		// MEM_result.val may have been written by LOAD. Do not overwrite it.
		MEM_result.pc = EX_result.pc;
		MEM_result.branch_flag = EX_result.branch_flag;
	}
	else if (EX_result.opcode == opcode::LOAD || EX_result.opcode == opcode::STORE)
		++MEM_cycle;
	else
	{
		MEM_result.opcode = EX_result.opcode;
		MEM_result.val = EX_result.val;
		MEM_result.reg = EX_result.reg;
		MEM_result.pc = EX_result.pc;
		MEM_result.branch_flag = EX_result.branch_flag;
	}
	MEM_stall = (MEM_cycle != 0);
}
