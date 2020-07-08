#ifndef INST_HPP
#define INST_HPP
const uint_fast8_t OP_IMM = 0x13, LOAD = 0x03, LUI = 0x37, AUIPC = 0x17, OP = 0x33, JAL = 0x6f, JALR = 0x67, BRANCH = 0x63, STORE = 0x23;
struct instruction
{
	uint_fast32_t orig;
	enum {I, U, R, J, B, S} type;
	uint32_t imm;
	uint_fast8_t rs1, rs2, rd;
	uint_fast8_t funct3, funct7;
	uint_fast8_t opcode;
	uint32_t oprand1, oprand2;
};
const instruction NOP {0x00000013, instruction::I, 0, 0, 0, 0, 0, 0, 0x13, 0, 0};
#endif
