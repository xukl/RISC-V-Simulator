#include <cstdint>
#include "inst.hpp"
extern const volatile MEM_inst MEM_result;
extern void end_of_simulation();

extern uint32_t reg[32];
extern bool reg_has_pending_write[32];
void WB()
{
	if (MEM_result.finish_flag)
		end_of_simulation();
	uint32_t val = MEM_result.val;
	uint_fast8_t rd = MEM_result.reg;
	switch (MEM_result.opcode)
	{
		case inst_opcode::JALR:
		case inst_opcode::JAL:
		case inst_opcode::OP_IMM:
		case inst_opcode::OP:
		case inst_opcode::LUI:
		case inst_opcode::LOAD:
		case inst_opcode::AUIPC:
			reg[rd] = val;
			reg_has_pending_write[rd] = false;
			break;
		case inst_opcode::BRANCH:
		case inst_opcode::STORE:
			;
	}
	reg[0] = 0;
}
