#include <cstdint>
#include "inst.hpp"
extern const volatile uint8_t memory[];
extern const volatile uint32_t pc;
extern const volatile jump_info jump_info_bus;
extern const volatile bool ID_stall;

extern IF_inst IF_result;

static bool end_reached = false;
static uint32_t IF_pc = 0;

void update_branch_predict()
{
}
bool branch_predict()
{
	return false;
}
constexpr inline uint32_t sign_ext_bit(int pos, uint32_t orig)
{
	return static_cast<uint32_t>(
			static_cast<int32_t>(orig & 0x80000000) >> (31 - pos));
}
void IF()
{
	if (jump_info_bus & jump_info::has_info)
	{
		if (!(jump_info_bus & jump_info::is_jump))
			update_branch_predict();
		if ((jump_info_bus & jump_info::mispredict) ||
				(jump_info_bus & jump_info::is_jump))
		{
			IF_pc = pc;
			end_reached = false;
		}
	}
	if (end_reached)
		IF_result = IF_NOP;
	else if (ID_stall)
	{
		IF_result = IF_NOP;
		IF_result.pc = IF_pc - 4;
	}
	else
	{
		uint32_t raw_inst = *reinterpret_cast<const volatile uint32_t*>(memory + IF_pc);
		IF_result = {raw_inst, IF_pc, jump_info(0)};
		if (inst_opcode(raw_inst & ((1 << 7) - 1)) == inst_opcode::BRANCH)
		{
			bool take_b = branch_predict();
			if (take_b)
			{
#define raw_bitmask(start_pos, len)\
	((raw_inst >> (start_pos)) & ((1 << (len)) - 1))
				uint32_t imm = 
					(raw_bitmask(7, 1) << 11) |
					(raw_bitmask(8, 4) << 1) |
					(raw_bitmask(25, 6) << 5) |
					sign_ext_bit(12, raw_inst);
#undef raw_bitmask
				IF_pc += imm;
				IF_result.j_info = jump_info(jump_info::has_info | jump_info::take_branch);
			}
			else
			{
				IF_pc += 4;
				IF_result.j_info = jump_info::has_info;
			}
			IF_result.j_info = jump_info::has_info;
		}
		else
			IF_pc += 4;
		if (IF_result.orig == END_INST_ORIG)
			end_reached = true;
	}
}
