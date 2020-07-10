#include <cstdint>
#include "inst.hpp"
extern const volatile ID_inst ID_result;
extern const volatile bool MEM_pause;

extern volatile uint32_t pc;
extern jump_info jump_info_bus;
extern EX_inst EX_result;
extern bool EX_stall;
extern bool EX_pause;
extern bool reg_has_pending_write[32];
void EX()
{
	if (MEM_pause)
	{
		EX_stall = true;
		return;
	}
	else
		EX_stall = false;
	EX_result.opcode = ID_result.opcode;
	switch (ID_result.format)
	{
		case inst_format::R:
		case inst_format::I:
		case inst_format::U:
		case inst_format::J:
			EX_result.reg = ID_result.rd;
			reg_has_pending_write[ID_result.rd] = true;
			reg_has_pending_write[0] = false;
			break;
		case inst_format::S:
			EX_result.reg = ID_result.rs2;
			break;
		case inst_format::B:
			;
	}
	EX_result.finish_flag = (ID_result.orig == 0x0ff00513);
	uint32_t rs1 = ID_result.rs1_val;
	uint32_t rs2 = ID_result.rs2_val;
	uint32_t imm = ID_result.imm;
	uint32_t ID_pc = ID_result.pc;
	switch (ID_result.exact_op)
	{
#define val_case(op_type, expr)\
		case inst_op::op_type:\
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
		val_case(LUI, imm)
		val_case(AUIPC, ID_pc + imm)
#undef val_case
#define SL_case(op_type, expr, info)\
		case inst_op::op_type:\
			EX_result.val = (expr);\
			EX_result.s_l_info = info;\
			break;
		SL_case(LB, rs1 + imm, -1)
		SL_case(LH, rs1 + imm, -2)
		SL_case(LW, rs1 + imm, -4)
		SL_case(LBU, rs1 + imm, 1)
		SL_case(LHU, rs1 + imm, 2)
		SL_case(SB, rs1 + imm, -1)
		SL_case(SH, rs1 + imm, -2)
		SL_case(SW, rs1 + imm, -4)
#undef SL_case
		default:
			;
	}
	switch (ID_result.exact_op)
	{
#define jump_case(op_type, expr)\
		case inst_op::op_type:\
			pc = (expr);\
			EX_result.val = ID_pc + 4;\
			EX_pause = true;\
			jump_info_bus = jump_info(jump_info::has_info | jump_info::is_jump);\
			break;
		jump_case(JAL, ID_pc + imm)
		jump_case(JALR, (rs1 + imm) & ~uint32_t(1))
#undef jump_case
#define branch_case(op_type, expr)\
		case inst_op::op_type:\
			if (expr)\
			{\
				if (!(ID_result.j_info & jump_info::take_branch))\
					EX_pause = true;\
				else\
					EX_pause = false;\
				pc = ID_pc + imm;\
			}\
			else\
			{\
				if (ID_result.j_info & jump_info::take_branch)\
					EX_pause = true;\
				else\
					EX_pause = false;\
				pc = ID_pc + 4;\
			}\
			jump_info_bus = jump_info(jump_info::has_info | ID_result.j_info | (EX_pause ? jump_info::mispredict : 0));\
			break;
		branch_case(BEQ, ID_pc + imm)
		branch_case(BNE, ID_pc + imm)
		branch_case(BLT, ID_pc + imm)
		branch_case(BGE, ID_pc + imm)
		branch_case(BLTU, ID_pc + imm)
		branch_case(BGEU, ID_pc + imm)
#undef branch_case
		default:
			pc = ID_pc + 4;
			EX_pause = false;
			jump_info_bus = jump_info(0);
	}
}
