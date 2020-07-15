#ifndef STATE_HPP
#define STATE_HPP
#include <cstdint>
#include "inst.hpp"
struct state
{
	uint32_t pc = 0;
	uint32_t reg[32];
	unsigned reg_has_pending_write[32];

	IF_inst IF_result = IF_NOP;
	ID_inst ID_result = ID_NOP;
	EX_inst EX_result = EX_NOP;
	MEM_inst MEM_result = MEM_NOP;
	bool IF_stall, ID_stall, EX_stall, MEM_stall;
	bool ID_pause, EX_pause, MEM_pause;
	bool mispredict;
};
struct btb_entry
{
	uint32_t pc = uint32_t(-1); // an impossible pc
	uint32_t target;
	int predict_state;
};
inline constexpr int MAX_DATA_MEMORY = 1 << 20;
inline constexpr int MAX_INST_MEMORY = 1 << 20;
inline constexpr int BIT_SIZE = 1024;
#endif
