#include <cstdint>
#include "inst.hpp"
#include "state.hpp"
extern const state old_state;
extern state new_state;
extern uint8_t d_memory[];

static uint8_t *const memory = d_memory;

static const EX_inst &EX_result = old_state.EX_result;
static MEM_inst &MEM_result = new_state.MEM_result;
static bool &MEM_stall = new_state.MEM_stall, &MEM_pause = new_state.MEM_pause;

static EX_inst EX_MEM_buff;

static int MEM_cycle = 0;
void MEM()
{
	auto &val = MEM_result.val;
	MEM_result.finish_flag = EX_result.finish_flag;
	if (MEM_cycle == 2)
	{
		switch (EX_MEM_buff.opcode)
		{
			case inst_opcode::LOAD:
				switch (EX_MEM_buff.s_l_info)
				{
					case -1:
						val = *reinterpret_cast<int8_t*>(memory + EX_MEM_buff.addr);
						break;
					case 1:
						val = *reinterpret_cast<uint8_t*>(memory + EX_MEM_buff.addr);
						break;
					case -2:
						val = *reinterpret_cast<int16_t*>(memory + EX_MEM_buff.addr);
						break;
					case 2:
						val = *reinterpret_cast<uint16_t*>(memory + EX_MEM_buff.addr);
						break;
					case -4:
						val = *reinterpret_cast<int32_t*>(memory + EX_MEM_buff.addr);
						break;
				}
				break;
			case inst_opcode::STORE:
				switch (EX_MEM_buff.s_l_info)
				{
					case -1:
						*reinterpret_cast<int8_t*>(memory + EX_MEM_buff.addr) = EX_MEM_buff.val;
						break;
					case -2:
						*reinterpret_cast<int16_t*>(memory + EX_MEM_buff.addr) = EX_MEM_buff.val;
						break;
					case -4:
						*reinterpret_cast<int32_t*>(memory + EX_MEM_buff.addr) = EX_MEM_buff.val;
						break;
				}
				break;
			default:
				;
		}
		MEM_cycle = 0;
		MEM_result.opcode = EX_MEM_buff.opcode;
		MEM_result.reg = EX_MEM_buff.reg;
		MEM_result.finish_flag = EX_MEM_buff.finish_flag;
	}
	else if (MEM_cycle == 1)
		++MEM_cycle;
	else if (EX_result.opcode == inst_opcode::LOAD || EX_result.opcode == inst_opcode::STORE)
	{
		++MEM_cycle;
		EX_MEM_buff = EX_result;
		MEM_result = MEM_NOP;
	}
	else
	{
		val = EX_result.val;
		MEM_result.opcode = EX_result.opcode;
		MEM_result.reg = EX_result.reg;
	}
	MEM_stall = (MEM_cycle != 0);
	MEM_pause = (MEM_cycle != 0);
}
