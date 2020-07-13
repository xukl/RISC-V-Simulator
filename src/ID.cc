#include <cstdint>
#include <cstdlib>
#include "inst.hpp"
extern const volatile uint32_t pc;
extern const volatile uint32_t reg[32];
extern const volatile bool reg_has_pending_write[32];
extern const volatile jump_info jump_info_bus;
extern const volatile IF_inst IF_result;
extern const volatile bool MEM_pause;

extern ID_inst ID_result;
extern bool ID_stall;
extern bool ID_pause;
constexpr inline uint32_t sign_ext(int sign_bit_pos, uint32_t orig)
{
	return static_cast<uint32_t>(
			(static_cast<int32_t>(orig) << (31 - sign_bit_pos))
			>> (31 - sign_bit_pos));
}
constexpr inline uint32_t sign_ext_bit(int pos, uint32_t orig)
{
	return static_cast<uint32_t>(
			static_cast<int32_t>(orig & 0x80000000) >> (31 - pos));
}

#define ID_bitmask(start_pos, len)\
	((ID_result.orig >> (start_pos)) & ((1 << (len)) - 1))

#define funct3_case(funct3_val, op)\
				case funct3_val:\
					ID_result.exact_op = inst_op::op;\
					break;
void instruction_R()
{
	ID_result.format = inst_format::R;
	ID_result.rd = ID_bitmask(7, 5);
	ID_result.funct3 = ID_bitmask(12, 3);
	ID_result.rs1 = ID_bitmask(15, 5);
	ID_result.rs2 = ID_bitmask(20, 5);
	ID_result.funct7 = ID_bitmask(25, 7);
	switch (ID_result.funct7)
	{
		case 0:
			switch (ID_result.funct3)
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
			switch (ID_result.funct3)
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
	ID_result.format = inst_format::I;
	ID_result.rd = ID_bitmask(7, 5);
	ID_result.funct3 = ID_bitmask(12, 3);
	ID_result.rs1 = ID_bitmask(15, 5);
	ID_result.rs2 = 0;
	ID_result.imm = sign_ext(11, ID_bitmask(20, 12));
	switch (ID_result.opcode)
	{
		case inst_opcode::JALR:
			if (ID_result.funct3 == 0)
				ID_result.exact_op = inst_op::JALR;
			else
				abort();
			break;
		case inst_opcode::LOAD:
			switch (ID_result.funct3)
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
		default: // case inst_opcode::OP_IMM:
			switch (ID_result.funct3)
			{
				funct3_case(0, ADDI)
				funct3_case(2, SLTI)
				funct3_case(3, SLTIU)
				funct3_case(4, XORI)
				funct3_case(6, ORI)
				funct3_case(7, ANDI)
				case 1:
					if (ID_result.imm >> 5 != 0)
						abort();
					ID_result.exact_op = inst_op::SLLI;
					break;
				case 5:
					if (ID_result.imm >> 5 == 0)
						ID_result.exact_op = inst_op::SRLI;
					else if (ID_result.imm >> 5 == 0x20)
						ID_result.exact_op = inst_op::SRAI;
					else
						abort();
			}
	}
}
void instruction_S()
{
	ID_result.format = inst_format::S;
	ID_result.rd = 0;
	ID_result.funct3 = ID_bitmask(12, 3);
	ID_result.rs1 = ID_bitmask(15, 5);
	ID_result.rs2 = ID_bitmask(20, 5);
	ID_result.imm = sign_ext(11, ID_bitmask(7, 5) | (ID_bitmask(25, 7) << 5));
	switch (ID_result.funct3)
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
	ID_result.format = inst_format::B;
	ID_result.rd = 0;
	ID_result.funct3 = ID_bitmask(12, 3);
	ID_result.rs1 = ID_bitmask(15, 5);
	ID_result.rs2 = ID_bitmask(20, 5);
	ID_result.imm =
		(ID_bitmask(7, 1) << 11) |
		(ID_bitmask(8, 4) << 1) |
		(ID_bitmask(25, 6) << 5) |
		sign_ext_bit(12, ID_result.orig);
	switch (ID_result.funct3)
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
	ID_result.format = inst_format::U;
	ID_result.rd = ID_bitmask(7, 5);
	ID_result.rs1 = 0;
	ID_result.rs2 = 0;
	ID_result.imm = ID_bitmask(12, 20) << 12;
	switch (ID_result.opcode)
	{
		case inst_opcode::LUI:
			ID_result.exact_op = inst_op::LUI;
			break;
		default: // case inst_opcode::AUIPC:
			ID_result.exact_op = inst_op::AUIPC;
	}
}
void instruction_J()
{
	ID_result.format = inst_format::J;
	ID_result.rd = ID_bitmask(7, 5);
	ID_result.rs1 = 0;
	ID_result.rs2 = 0;
	ID_result.imm =
		(ID_bitmask(12, 8) << 12) |
		(ID_bitmask(20, 1) << 11) |
		(ID_bitmask(21, 10) << 1) |
		sign_ext_bit(20, ID_result.orig);
	ID_result.exact_op = inst_op::JAL;
}
#undef funct3_case
void ID()
{
	if (MEM_pause)
	{
		ID_stall = true;
		return;
	}
	if ((jump_info_bus & jump_info::has_info) &&
			((jump_info_bus & jump_info::mispredict) ||
			 (jump_info_bus & jump_info::is_jump)))
	{
		ID_stall = true;
		ID_pause = false;
		ID_result = ID_NOP;
		ID_result.pc = pc - 4;
		return;
	}
	else
	{
		ID_result.orig = IF_result.orig;
		ID_result.opcode = inst_opcode(ID_bitmask(0, 7));
		switch (ID_result.opcode)
		{
			case inst_opcode::OP:
				instruction_R();
				break;
			case inst_opcode::OP_IMM:
			case inst_opcode::LOAD:
			case inst_opcode::JALR:
				instruction_I();
				break;
			case inst_opcode::STORE:
				instruction_S();
				break;
			case inst_opcode::BRANCH:
				instruction_B();
				break;
			case inst_opcode::LUI:
			case inst_opcode::AUIPC:
				instruction_U();
				break;
			case inst_opcode::JAL:
				instruction_J();
				break;
			default:
				abort();
		}
		ID_pause =
			reg_has_pending_write[ID_result.rs1] ||
			reg_has_pending_write[ID_result.rs2];
	}
	if (!ID_pause)
	{
		ID_stall = false;
		ID_result.rs1_val = reg[ID_result.rs1];
		ID_result.rs2_val = reg[ID_result.rs2];
		ID_result.pc = IF_result.pc;
	}
	else
	{
		ID_stall = true;
		ID_result = ID_NOP;
		ID_result.pc = IF_result.pc - 4;
	}
}
#undef ID_bitmask
