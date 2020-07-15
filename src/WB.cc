#include <cstdint>
#include "inst.hpp"
#include "state.hpp"
extern const state old_state;
extern state new_state;
static const MEM_inst &MEM_result = old_state.MEM_result;
void end_of_simulation();

static uint32_t *const reg = new_state.reg;
static unsigned *const reg_has_pending_write = new_state.reg_has_pending_write;
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
			if (rd != 0)
				--reg_has_pending_write[rd];
			break;
		case inst_opcode::BRANCH:
		case inst_opcode::STORE:
			;
	}
	reg[0] = 0;
}
