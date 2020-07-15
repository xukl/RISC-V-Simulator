#include "state.hpp"
state old_state, new_state;
uint8_t d_memory[MAX_DATA_MEMORY];
uint32_t i_memory[MAX_INST_MEMORY];
btb_entry btb[BIT_SIZE];

int BP_cnt_fail, BP_cnt_success;
