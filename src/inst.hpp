#ifndef INST_HPP
#define INST_HPP
const uint_fast8_t OP_IMM = 0x13, LOAD = 0x03, LUI = 0x37, AUIPC = 0x17, OP = 0x33, JAL = 0x6f, JALR = 0x67, BRANCH = 0x63, STORE = 0x23;
struct ID_inst
{
	uint_fast32_t orig;
	enum {I, U, R, J, B, S} type;
	uint32_t imm;
	uint_fast8_t rs1, rs2, rd;
	uint_fast8_t funct3, funct7;
	uint_fast8_t opcode;
	uint32_t rs1_val, rs2_val;
	enum {ADD, SLT, SLTU, AND, OR, XOR, SLL, SRL, SUB, SRA, ADDI, SLTI, SLTIU, ANDI, ORI, XORI, SLLI, SRLI, SRAI, LB, LH, LW, LBU, LHU, SB, SH, SW, BEQ, BNE, BLT, BLTU, BGE, BGEU, LUI, AUIPC, JAL, JALR} exact_op;
	uint8_t *pc;
};
const uint32_t NOP_orig = 0x00000013;
const ID_inst NOP {NOP_orig, ID_inst::I, 0, 0, 0, 0, 0, 0, OP_IMM, 0, 0, ID_inst::ADDI, nullptr};
struct EX_inst
{
	uint_fast8_t opcode;
	uint32_t val;
	uint_fast8_t reg;
	uint32_t *pc;
	bool branch_flag;
};
struct MEM_inst {};
#endif
