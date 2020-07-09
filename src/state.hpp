#ifndef STATE_HPP
#define STATE_HPP
#include <cstdint>
#include "inst.hpp"
uint32_t pc = 0;
uint32_t reg[32];
bool reg_has_pending_write[32];
const int MAX_MEMORY = 0x20 << 12;
uint8_t memory[MAX_MEMORY];

uint32_t IF_result;
ID_inst ID_result;
EX_inst EX_result;
MEM_inst MEM_result;
bool ID_stall;
#endif
