#include <cstdint>
#include "inst.hpp"
uint32_t pc = 0;
uint32_t reg[32];
bool reg_has_pending_write[32];
const int MAX_MEMORY = 0x20 << 12;
uint8_t memory[MAX_MEMORY];

uint32_t IF_result = IF_NOP;
ID_inst ID_result = ID_NOP;
EX_inst EX_result = EX_NOP;
MEM_inst MEM_result = MEM_NOP;
bool ID_stall, EX_stall, MEM_stall;
