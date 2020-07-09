#include <cstdint>
#include "inst.hpp"

extern const volatile uint32_t reg[32];
extern uint8_t memory[];

extern const volatile EX_inst EX_result;
extern MEM_inst MEM_result;
extern bool MEM_stall;

static int MEM_cycle = 0;
void MEM()
{
	auto &val = MEM_result.val;
	MEM_result.finish_flag = EX_result.finish_flag;
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
		default:
			MEM_result.val = EX_result.val;
	}
	MEM_cycle = 0;
	MEM_result.opcode = EX_result.opcode;
	MEM_result.reg = EX_result.reg;
	// MEM_result.val may have been written by LOAD. Do not overwrite it.
	MEM_result.pc = EX_result.pc;
	MEM_result.branch_flag = EX_result.branch_flag;
	MEM_stall = (MEM_cycle != 0);
}
