#ifndef INST_HPP
#define INST_HPP
#include <cstdint>
inline constexpr uint32_t END_INST_ORIG = 0x0ff00513;
struct IF_inst
{
	uint32_t orig;
	uint32_t pc;
	bool jumped;
};
inline constexpr IF_inst IF_NOP {0x00000013, uint32_t(-4), false};
enum class inst_opcode {OP_IMM = 0b0010011, LOAD = 0b0000011, JALR = 0b1100111, LUI = 0b0110111, AUIPC = 0b0010111, OP = 0b0110011, JAL = 0b1101111, BRANCH = 0b1100011, STORE = 0b0100011};
enum class inst_format {I, U, R, J, B, S};
enum class inst_op {ADD, SLT, SLTU, AND, OR, XOR, SLL, SRL, SUB, SRA, MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU, ADDI, SLTI, SLTIU, ANDI, ORI, XORI, SLLI, SRLI, SRAI, LB, LH, LW, LBU, LHU, SB, SH, SW, BEQ, BNE, BLT, BLTU, BGE, BGEU, LUI, AUIPC, JAL, JALR};
struct ID_inst
{
	uint32_t orig;
	inst_format format;
	uint32_t imm;
	uint_fast8_t rs1, rs2, rd;
	uint_fast8_t funct3, funct7;
	inst_opcode opcode;
	uint32_t rs1_val, rs2_val;
	inst_op exact_op;
	uint32_t pc;
	bool jumped;
};
inline constexpr ID_inst ID_NOP {IF_NOP.orig, inst_format::I, 0, 0, 0, 0, 0, 0, inst_opcode::OP_IMM, 0, 0, inst_op::ADDI, uint32_t(-4), false};
struct EX_inst
{
	bool finish_flag;
	inst_opcode opcode;
	uint32_t val, addr;
	uint_fast8_t reg;
	int s_l_info;
};
inline constexpr EX_inst EX_NOP {false, inst_opcode::OP_IMM, 0, 0, 0, 0};
struct MEM_inst
{
	bool finish_flag;
	inst_opcode opcode;
	uint32_t val;
	uint_fast8_t reg;
};
inline constexpr MEM_inst MEM_NOP {false, inst_opcode::OP_IMM, 0, 0};
#endif
