#include <cstdint>
#include <cstdlib>
#include "common.hpp"
#include "state.hpp"

instruction ID_pending_result;
constexpr inline uint32_t sign_ext_bit(int pos, uint32_t orig)
{
	return static_cast<uint32_t>(
			static_cast<int32_t>(orig & 0x80000000) >> (31 - pos));
}
#define bitmask(start_pos, len)\
	((ID_pending_result.orig >> (start_pos)) & ((1 << (len)) - 1))
void instruction_R()
{
	ID_pending_result.type = instruction::R;
	ID_pending_result.rd = bitmask(7, 5);
	ID_pending_result.funct3 = bitmask(12, 3);
	ID_pending_result.rs1 = bitmask(15, 5);
	ID_pending_result.rs2 = bitmask(20, 5);
	ID_pending_result.funct7 = bitmask(25, 7);
}
void instruction_I()
{
	ID_pending_result.type = instruction::I;
	ID_pending_result.rd = bitmask(7, 5);
	ID_pending_result.funct3 = bitmask(12, 3);
	ID_pending_result.rs1 = bitmask(15, 5);
	ID_pending_result.rs2 = 0;
	ID_pending_result.imm = sign_ext(11, bitmask(20, 12));
}
void instruction_S()
{
	ID_pending_result.type = instruction::S;
	ID_pending_result.rd = 0;
	ID_pending_result.funct3 = bitmask(12, 3);
	ID_pending_result.rs1 = bitmask(15, 5);
	ID_pending_result.rs2 = bitmask(20, 5);
	ID_pending_result.imm = sign_ext(11, bitmask(7, 5) | (bitmask(25, 7) << 5));
}
void instruction_B()
{
	ID_pending_result.type = instruction::B;
	ID_pending_result.rd = 0;
	ID_pending_result.funct3 = bitmask(12, 3);
	ID_pending_result.rs1 = bitmask(15, 5);
	ID_pending_result.rs2 = bitmask(20, 5);
	ID_pending_result.imm =
		(bitmask(7, 1) << 11) |
		(bitmask(8, 4) << 1) |
		(bitmask(25, 6) << 5) |
		sign_ext_bit(12, ID_pending_result.orig);
}
void instruction_U()
{
	ID_pending_result.type = instruction::U;
	ID_pending_result.rd = bitmask(7, 5);
	ID_pending_result.rs1 = 0;
	ID_pending_result.rs2 = 0;
	ID_pending_result.imm = bitmask(12, 20) << 12;
}
void instruction_J()
{
	ID_pending_result.type = instruction::J;
	ID_pending_result.rd = bitmask(7, 5);
	ID_pending_result.rs1 = 0;
	ID_pending_result.rs2 = 0;
	ID_pending_result.imm =
		(bitmask(12, 8) << 12) |
		(bitmask(20, 1) << 11) |
		(bitmask(21, 10) << 1) |
		sign_ext_bit(20, ID_pending_result.orig);
}
#undef bitmask
void ID()
{
	if (!ID_stall)
	{
		ID_pending_result.orig = IF_result.orig;
		ID_pending_result.opcode = ID_pending_result.orig & ((1 << 7) - 1);
		switch (ID_pending_result.opcode)
		{
			case OP:
				instruction_R();
				break;
			case OP_IMM:
			case LOAD:
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
			case JALR:
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
	}
	else
		ID_result = NOP;
}
