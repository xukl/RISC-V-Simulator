#include <cstdint>
#include <cstdlib>
#include "inst.hpp"
#include "state.hpp"
extern const state old_state;
extern state new_state;

static const ID_inst &ID_result = old_state.ID_result;
static const bool &MEM_pause = old_state.MEM_pause;

static uint32_t &pc = new_state.pc;
static bool &mispredict = new_state.mispredict;
static EX_inst &EX_result = new_state.EX_result;
static bool &EX_stall = new_state.EX_stall;
static bool &EX_pause = new_state.EX_pause;
static bool *const reg_has_pending_write = new_state.reg_has_pending_write;
extern const int BIT_SIZE;
extern btb_entry btb[];

extern int BP_cnt_fail, BP_cnt_success;
void update_btb(uint32_t inst_pc, uint32_t branch_target, bool take, bool mispredict)
{
	if (mispredict)
		++BP_cnt_fail;
	else
		++BP_cnt_success;
	btb_entry &entry = btb[(inst_pc / 4) & (BIT_SIZE - 1)];
	if (entry.pc != inst_pc)
	{
		entry.pc = inst_pc;
		entry.target = branch_target;
		if (take)
			entry.predict_state = 3;
		else
			entry.predict_state = 0;
		return;
	}
	if (take)
	{
		if (entry.predict_state != 3)
			++entry.predict_state;
	}
	else
	{
		if (entry.predict_state != 0)
			--entry.predict_state;
	}
}
void EX()
{
	if (old_state.EX_pause)
	{
		EX_pause = false;
		EX_stall = false;
		if (!MEM_pause)
			EX_result = EX_NOP;
		mispredict = false;
		return;
	}
	if (MEM_pause)
	{
		EX_stall = true;
		return;
	}
	EX_stall = false;
	EX_result.opcode = ID_result.opcode;
	if (EX_result.opcode == inst_opcode(0)) // wrong_inst_fmt
		abort();
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
#define SL_case(op_type, info)\
		case inst_op::op_type:\
			EX_result.val = rs2;\
			EX_result.addr = rs1 + imm;\
			EX_result.s_l_info = info;\
			break;
		SL_case(LB, -1)
		SL_case(LH, -2)
		SL_case(LW, -4)
		SL_case(LBU, 1)
		SL_case(LHU, 2)
		SL_case(SB, -1)
		SL_case(SH, -2)
		SL_case(SW, -4)
#undef SL_case
		default:
			;
	}
	switch (ID_result.exact_op)
	{
		case inst_op::JAL:
			pc = ID_pc + imm;
			EX_result.val = ID_pc + 4;
			if (!ID_result.jumped)
				EX_pause = true;
			mispredict = EX_pause;
			update_btb(ID_pc, ID_pc + imm, true, mispredict);
			break;
		case inst_op::JALR:
			pc = ((rs1 + imm) & -1);
			EX_result.val = ID_pc + 4;
			EX_pause = true;
			mispredict = true;
			break;
#define branch_case(op_type, expr)\
		case inst_op::op_type:\
			if (expr)\
			{\
				if (ID_result.jumped)\
					EX_pause = false;\
				else\
					EX_pause = true;\
				pc = ID_pc + imm;\
			}\
			else\
			{\
				if (ID_result.jumped)\
					EX_pause = true;\
				else\
					EX_pause = false;\
				pc = ID_pc + 4;\
			}\
			mispredict = EX_pause;\
			update_btb(ID_pc, ID_pc + imm, expr, mispredict);\
			break;
		branch_case(BEQ, rs1 == rs2)
		branch_case(BNE, rs1 != rs2)
		branch_case(BLT, int32_t(rs1) < int32_t(rs2))
		branch_case(BGE, int32_t(rs1) >= int32_t(rs2))
		branch_case(BLTU, uint32_t(rs1) < uint32_t(rs2))
		branch_case(BGEU, uint32_t(rs1) >= uint32_t(rs2))
#undef branch_case
		default:
			pc = ID_pc + 4;
			EX_pause = false;
			mispredict = false;
	}
}
