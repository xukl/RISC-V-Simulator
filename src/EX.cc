#include <cstdint>
#include "state.hpp"

void EX()
{
	EX_result.opcode = ID_result.opcode;
	EX_result.pc = ID_result.pc;
	switch (ID_result.type)
	{
		case ID_inst::R:
		case ID_inst::I:
		case ID_inst::U:
		case ID_inst::J:
			EX_result.reg = ID_result.rd;
			break;
		case ID_inst::S:
			EX_result.reg = ID_result.rs1;
			break;
		case ID_inst::B:
			;
	}
	uint32_t rs1 = ID_result.rs1_val;
	uint32_t rs2 = ID_result.rs2_val;
	uint32_t imm = ID_result.imm;
	switch (ID_result.exact_op)
	{
#define val_case(op_type, expr)\
		case ID_inst::op_type:\
			EX_result.val = (expr);\
		break;
		val_case(ADD, rs1 + rs2)
		val_case(SLT, (int32_t(rs1) < int32_t(rs2)) ? 1 : 0)
		val_case(SLTU, (uint32_t(rs1) < uint32_t(rs2)) ? 1 : 0)
		val_case(AND, rs1 & rs2)
		val_case(OR, rs1 | rs2)
		val_case(XOR, rs1 ^ rs2)
		val_case(SLL, uint32_t(rs1) << (rs2 & 0x1f))
		val_case(SRL, uint32_t(rs1) >> (rs2 & 0x1f))
		val_case(SUB, rs1 - rs2)
		val_case(SRA, int32_t(rs1) >> (rs2 & 0x1f))
		val_case(ADDI, rs1 + imm)
		val_case(SLTI, (int32_t(rs1) < int32_t(imm)) ? 1 : 0)
		val_case(SLTIU, (uint32_t(rs1) < uint32_t(imm)) ? 1 : 0)
		val_case(ANDI, rs1 & imm)
		val_case(ORI, rs1 | imm)
		val_case(XORI, rs1 ^ imm)
		val_case(SLLI, uint32_t(rs1) << (imm & 0x1f))
		val_case(SRLI, uint32_t(rs1) >> (imm & 0x1f))
		val_case(SRAI, int32_t(rs1) >> (imm & 0x1f))
		val_case(LB, rs1 + imm)
		val_case(LH, rs1 + imm)
		val_case(LW, rs1 + imm)
		val_case(LBU, rs1 + imm)
		val_case(LHU, rs1 + imm)
		val_case(JALR, (rs1 + imm) & ~uint32_t(1))
		val_case(SB, rs1 + imm)
		val_case(SH, rs1 + imm)
		val_case(SW, rs1 + imm)
		val_case(BEQ, pc + imm)
		val_case(BNE, pc + imm)
		val_case(BLT, pc + imm)
		val_case(BGE, pc + imm)
		val_case(BLTU, pc + imm)
		val_case(BGEU, pc + imm)
		val_case(LUI, imm)
		val_case(AUIPC, pc + imm)
		val_case(JAL, pc + imm)
#undef val_case
	}
	switch (ID_result.exact_op)
	{
#define flag_case(op_type, expr)\
		case ID_inst::op_type:\
			EX_result.branch_flag = (expr);\
			break;
		flag_case(BEQ, rs1 == rs2)
		flag_case(BNE, rs1 != rs2)
		flag_case(BLT, int32_t(rs1) < int32_t(rs2))
		flag_case(BGE, int32_t(rs1) >= int32_t(rs2))
		flag_case(BLTU, uint32_t(rs1) < uint32_t(rs2))
		flag_case(BGEU, uint32_t(rs1) >= uint32_t(rs2))
#undef flag_case
		default:
			;
	}
}
