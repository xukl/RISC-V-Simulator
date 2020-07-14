#include "state.hpp"
state old_state, new_state;
const static int MAX_DATA_MEMORY = 1 << 20;
uint8_t d_memory[MAX_DATA_MEMORY];
const static int MAX_INST_MEMORY = 1 << 20;
uint32_t i_memory[MAX_INST_MEMORY];
