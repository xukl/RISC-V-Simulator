#ifndef INST_HPP
#define INST_HPP
enum class inst_opcode // to use opcode::SOME_OP like it were an enum class
{
	OP_IMM = 0x13, LOAD = 0x03, LUI = 0x37, AUIPC = 0x17, OP = 0x33, JAL = 0x6f, JALR = 0x67, BRANCH = 0x63, STORE = 0x23
};
enum class inst_format {I, U, R, J, B, S};
enum class inst_op {ADD, SLT, SLTU, AND, OR, XOR, SLL, SRL, SUB, SRA, ADDI, SLTI, SLTIU, ANDI, ORI, XORI, SLLI, SRLI, SRAI, LB, LH, LW, LBU, LHU, SB, SH, SW, BEQ, BNE, BLT, BLTU, BGE, BGEU, LUI, AUIPC, JAL, JALR};
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
};
static const uint32_t IF_NOP = 0x00000013;
static const ID_inst ID_NOP {IF_NOP, inst_format::I, 0, 0, 0, 0, 0, 0, inst_opcode::OP_IMM, 0, 0, inst_op::ADDI, uint32_t(-4)};
struct EX_inst
{
	bool finish_flag;
	inst_opcode opcode;
	uint32_t val;
	uint_fast8_t reg;
	uint32_t pc;
	bool branch_flag;
	int s_l_info;
};
static const EX_inst EX_NOP {false, inst_opcode::OP_IMM, 0, 0, uint32_t(-4), false, 0};
struct MEM_inst
{
	bool finish_flag;
	inst_opcode opcode;
	uint32_t val;
	uint_fast8_t reg;
	uint32_t pc;
	bool branch_flag;
};
static const MEM_inst MEM_NOP {false, inst_opcode::OP_IMM, 0, 0, uint32_t(-4), false};
#endif
