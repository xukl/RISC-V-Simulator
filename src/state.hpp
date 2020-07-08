#ifndef STATE_HPP
#define STATE_HPP
#include <cstdint>
#include "inst.hpp"
uint8_t *pc = nullptr;
uint32_t reg[32];
bool reg_has_pending_write[32];
const int MAX_MEMORY = 0x20 << 12;
uint8_t memory[MAX_MEMORY];

instruction IF_result, ID_result, EX_result, MEM_result;
bool ID_stall;
#endif
