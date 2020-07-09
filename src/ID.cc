#include <cstdint>
#include <cstdlib>
#include "common.hpp"
#include "state.hpp"

static ID_inst ID_pending_result;
#define ID_bitmask(start_pos, len)\
	((ID_pending_result.orig >> (start_pos)) & ((1 << (len)) - 1))

#define funct3_case(funct3_val, op)\
				case funct3_val:\
					ID_pending_result.exact_op = ID_inst::op;\
					break;
void instruction_R()
{
	ID_pending_result.type = ID_inst::R;
	ID_pending_result.rd = ID_bitmask(7, 5);
	ID_pending_result.funct3 = ID_bitmask(12, 3);
	ID_pending_result.rs1 = ID_bitmask(15, 5);
	ID_pending_result.rs2 = ID_bitmask(20, 5);
	ID_pending_result.funct7 = ID_bitmask(25, 7);
	switch (ID_pending_result.funct7)
	{
		case 0:
			switch (ID_pending_result.funct3)
			{
				funct3_case(0, ADD)
				funct3_case(1, SLL)
				funct3_case(2, SLT)
				funct3_case(3, SLTU)
				funct3_case(4, XOR)
				funct3_case(5, SRL)
				funct3_case(6, OR)
				funct3_case(7, AND)
			}
			break;
		case 0x20:
			switch (ID_pending_result.funct3)
			{
				funct3_case(0, SUB)
				funct3_case(5, SRA)
				default:
					abort();
			}
			break;
		default:
			abort();
	}
}
void instruction_I()
{
	ID_pending_result.type = ID_inst::I;
	ID_pending_result.rd = ID_bitmask(7, 5);
	ID_pending_result.funct3 = ID_bitmask(12, 3);
	ID_pending_result.rs1 = ID_bitmask(15, 5);
	ID_pending_result.rs2 = 0;
	ID_pending_result.imm = sign_ext(11, ID_bitmask(20, 12));
	switch (ID_pending_result.opcode)
	{
		case JALR:
			if (ID_pending_result.funct3 == 0)
				ID_pending_result.exact_op = ID_inst::JALR;
			else
				abort();
			break;
		case LOAD:
			switch (ID_pending_result.funct3)
			{
				funct3_case(0, LB)
				funct3_case(1, LH)
				funct3_case(2, LW)
				funct3_case(4, LBU)
				funct3_case(5, LHU)
				default:
					abort();
			}
			break;
		default: // case OP_IMM:
			switch (ID_pending_result.funct3)
			{
				funct3_case(0, ADDI)
				funct3_case(2, SLTI)
				funct3_case(3, SLTIU)
				funct3_case(4, XORI)
				funct3_case(6, ORI)
				funct3_case(7, ANDI)
				case 1:
					if (ID_pending_result.imm >> 5 != 0)
						abort();
					ID_pending_result.exact_op = ID_inst::SLLI;
					break;
				case 5:
					if (ID_pending_result.imm >> 5 == 0)
						ID_pending_result.exact_op = ID_inst::SRLI;
					else if (ID_pending_result.imm >> 5 == 0x20)
						ID_pending_result.exact_op = ID_inst::SRAI;
					else
						abort();
			}
	}
}
void instruction_S()
{
	ID_pending_result.type = ID_inst::S;
	ID_pending_result.rd = 0;
	ID_pending_result.funct3 = ID_bitmask(12, 3);
	ID_pending_result.rs1 = ID_bitmask(15, 5);
	ID_pending_result.rs2 = ID_bitmask(20, 5);
	ID_pending_result.imm = sign_ext(11, ID_bitmask(7, 5) | (ID_bitmask(25, 7) << 5));
	switch (ID_pending_result.funct3)
	{
		funct3_case(0, SB)
		funct3_case(1, SH)
		funct3_case(2, SW)
		default:
			abort();
	}
}
void instruction_B()
{
	ID_pending_result.type = ID_inst::B;
	ID_pending_result.rd = 0;
	ID_pending_result.funct3 = ID_bitmask(12, 3);
	ID_pending_result.rs1 = ID_bitmask(15, 5);
	ID_pending_result.rs2 = ID_bitmask(20, 5);
	ID_pending_result.imm =
		(ID_bitmask(7, 1) << 11) |
		(ID_bitmask(8, 4) << 1) |
		(ID_bitmask(25, 6) << 5) |
		sign_ext_bit(12, ID_pending_result.orig);
	switch (ID_pending_result.funct3)
	{
		funct3_case(0, BEQ)
		funct3_case(1, BNE)
		funct3_case(4, BLT)
		funct3_case(5, BGE)
		funct3_case(6, BLTU)
		funct3_case(7, BGEU)
		default:
			abort();
	}
}
void instruction_U()
{
	ID_pending_result.type = ID_inst::U;
	ID_pending_result.rd = ID_bitmask(7, 5);
	ID_pending_result.rs1 = 0;
	ID_pending_result.rs2 = 0;
	ID_pending_result.imm = ID_bitmask(12, 20) << 12;
	switch (ID_pending_result.opcode)
	{
		case LUI:
			ID_pending_result.exact_op = ID_inst::LUI;
			break;
		default: // case AUIPC:
			ID_pending_result.exact_op = ID_inst::AUIPC;
	}
}
void instruction_J()
{
	ID_pending_result.type = ID_inst::J;
	ID_pending_result.rd = ID_bitmask(7, 5);
	ID_pending_result.rs1 = 0;
	ID_pending_result.rs2 = 0;
	ID_pending_result.imm =
		(ID_bitmask(12, 8) << 12) |
		(ID_bitmask(20, 1) << 11) |
		(ID_bitmask(21, 10) << 1) |
		sign_ext_bit(20, ID_pending_result.orig);
	ID_pending_result.exact_op = ID_inst::JAL;
}
#undef funct3_case
void ID()
{
	if (!ID_stall)
	{
		ID_pending_result.orig = IF_result;
		ID_pending_result.opcode = ID_bitmask(0, 7);
		switch (ID_pending_result.opcode)
		{
			case OP:
				instruction_R();
				break;
			case OP_IMM:
			case LOAD:
			case JALR:
				instruction_I();
				break;
			case STORE:
				instruction_S();
				break;
			case BRANCH:
				instruction_B();
				break;
			case LUI:
			case AUIPC:
				instruction_U();
				break;
			case JAL:
				instruction_J();
				break;
			default:
				abort();
		}
	}
	ID_stall =
		reg_has_pending_write[ID_pending_result.rs1] ||
		reg_has_pending_write[ID_pending_result.rs2];
	if (!ID_stall)
	{
		reg_has_pending_write[ID_pending_result.rd] = true;
		reg_has_pending_write[0] = false;
		ID_result = ID_pending_result;
		ID_result.rs1_val = reg[ID_result.rs1];
		ID_result.rs2_val = reg[ID_result.rs2];
		ID_result.pc = pc;
	}
	else
	{
		ID_result = NOP;
		ID_result.pc = pc - 4;
	}
}
#undef ID_bitmask
