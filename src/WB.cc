#include <cstdint>
#include "inst.hpp"

extern uint32_t pc;
extern uint32_t reg[32];
extern bool reg_has_pending_write[32];

extern const volatile MEM_inst MEM_result;
extern const volatile bool MEM_stall;

extern void end_of_simulation();

void WB()
{
	if (MEM_result.finish_flag)
		end_of_simulation();
	uint32_t prev_pc = MEM_result.pc;
	uint32_t val = MEM_result.val;
	uint_fast8_t rd = MEM_result.reg;
	switch (MEM_result.opcode)
	{
		case inst_opcode::JALR:
		case inst_opcode::JAL:
			reg[rd] = prev_pc + 4;
			reg_has_pending_write[rd] = false;
			pc = val;
			break;
		case inst_opcode::OP_IMM:
		case inst_opcode::OP:
		case inst_opcode::LUI:
		case inst_opcode::LOAD:
		case inst_opcode::AUIPC:
			reg[rd] = val;
			reg_has_pending_write[rd] = false;
			pc = prev_pc + 4;
			break;
		case inst_opcode::BRANCH:
			if (MEM_result.branch_flag)
				pc = val;
			else
				pc = prev_pc + 4;
			break;
		case inst_opcode::STORE:
			pc = prev_pc + 4;
	}
	reg[0] = 0;
}
