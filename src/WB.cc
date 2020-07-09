#include <cstdint>
#include "state.hpp"
#include "ending.hpp"

void WB()
{
	if (MEM_stall)
		return;
	if (MEM_result.finish_flag)
		end_of_simulation();
	uint32_t prev_pc = MEM_result.pc;
	uint32_t val = MEM_result.val;
	uint_fast8_t rd = MEM_result.reg;
	switch (MEM_result.opcode)
	{
		case opcode::JALR:
		case opcode::JAL:
			reg[rd] = prev_pc + 4;
			pc = val;
			break;
		case opcode::OP_IMM:
		case opcode::OP:
		case opcode::LUI:
		case opcode::LOAD:
			reg[rd] = val;
			pc = prev_pc + 4;
			break;
		case opcode::BRANCH:
			if (MEM_result.branch_flag)
				pc = val;
			else
				pc = prev_pc + 4;
	}
	reg[0] = 0;
}
