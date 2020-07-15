#include <cstdint>
#include "inst.hpp"
#include "state.hpp"
extern const uint8_t i_memory[];
extern const state old_state;
extern state new_state;

static const uint32_t &pc = old_state.pc;
static const bool &mispredict = old_state.mispredict;
static const bool &ID_pause = old_state.ID_pause, &MEM_pause = old_state.MEM_pause;
extern const int BIT_SIZE;
extern const btb_entry btb[];

static IF_inst &IF_result = new_state.IF_result;
static bool &IF_stall = new_state.IF_stall;

static bool end_reached = false;
static uint32_t IF_pc = 0;

uint32_t branch_target_predict()
{
	btb_entry entry = btb[(IF_pc / 4) % BIT_SIZE];
	if (entry.pc != IF_pc || entry.predict_state < 2)
		return IF_pc + 4;
	return entry.target;
}

void IF()
{
	if (mispredict)
	{
		IF_pc = pc;
		end_reached = false;
		goto fetch;
	}
	if (MEM_pause || ID_pause)
	{
		IF_stall = true;
		return;
	}
	if (end_reached)
	{
		IF_result = IF_NOP;
		IF_stall = true;
	}
	else
	{
fetch:
		IF_stall = false;
		uint32_t raw_inst = *reinterpret_cast<const uint32_t*>(i_memory + IF_pc);
		IF_result = {raw_inst, IF_pc, false};
		uint32_t predict_pc = branch_target_predict();
		if (predict_pc != IF_pc + 4)
			IF_result.jumped = true;
		IF_pc = predict_pc;
		if (IF_result.orig == END_INST_ORIG)
			end_reached = true;
	}
}
