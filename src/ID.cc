#include <cstdint>
#include "inst.hpp"
#include "state.hpp"
extern const state old_state;
extern state new_state;

static const uint32_t &pc = old_state.pc;
static const uint32_t *const reg = old_state.reg;
static const unsigned *const reg_has_pending_write = old_state.reg_has_pending_write;
static const bool &mispredict = old_state.mispredict;
static const IF_inst &IF_result = old_state.IF_result;
static const bool &MEM_pause = old_state.MEM_pause;

static ID_inst &ID_result = new_state.ID_result;
static bool &ID_stall = new_state.ID_stall;
static bool &ID_pause = new_state.ID_pause;

static uint_fast8_t last_rd;
static IF_inst IF_ID_buff;

/* function: sign_ext
 * input: sign_bit_pos, orig
 *	orig (binary):
 * 	31                                    0
 * 	v                                     v
 * 	zzzz zzXy yyyy yyyy yyyy yyyy yyyy yyyy
 * 	       ^
 * 	       sign_bit_pos
 *
 * output (binary):
 * 	31                                    0
 * 	v                                     v
 * 	XXXX XXXy yyyy yyyy yyyy yyyy yyyy yyyy
 * 	       ^
 * 	       sign_bit_pos
 */
constexpr inline uint32_t sign_ext(int sign_bit_pos, uint32_t orig)
{
	return static_cast<uint32_t>(
			(static_cast<int32_t>(orig) << (31 - sign_bit_pos))
			>> (31 - sign_bit_pos));
}

/* function: sign_ext_bit
 * input: pos, orig
 *	orig (binary):
 * 	31                                    0
 * 	v                                     v
 * 	Xyyy yyyy yyyy yyyy yyyy yyyy yyyy yyyy
 *
 * output (binary):
 * 	31                                    0
 * 	v                                     v
 * 	XXXX XXXX XXX0 0000 0000 0000 0000 0000
 * 	            ^
 * 	            pos
 */
constexpr inline uint32_t sign_ext_bit(int pos, uint32_t orig)
{
	return static_cast<uint32_t>(
			static_cast<int32_t>(orig & 0x80000000) >> (31 - pos));
}

/* function: ID_bitmask
 * input: start_pos, len
 *	ID_result.orig (binary):
 * 	31                                    0
 * 	v                                     v
 * 	xxxX XXXX XXXx xxxx xxxx xxxx xxxx xxxx
 * 	   ^        ^
 * 	   |        start_pos
 * 	   (start_pos + len - 1)
 *
 * output (binary):
 * 	31                                    0
 * 	v                                     v
 * 	0000 0000 0000 0000 0000 0000 XXXX XXXX
 * 	                              ^
 * 	                              (len - 1)
 */
#define ID_bitmask(start_pos, len)\
	((ID_result.orig >> (start_pos)) & ((1 << (len)) - 1))

#define funct3_case(funct3_val, op)\
				case funct3_val:\
					ID_result.exact_op = inst_op::op;\
					break;

inline void complain_wrong_inst_fmt()
{
	ID_result.opcode = inst_opcode(0);
}
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
		case 1:
			switch (ID_result.funct3)
			{
				funct3_case(0, MUL)
				funct3_case(1, MULH)
				funct3_case(2, MULHSU)
				funct3_case(3, MULHU)
				funct3_case(4, DIV)
				funct3_case(5, DIVU)
				funct3_case(6, REM)
				funct3_case(7, REMU)
			}
			break;
		case 0x20:
			switch (ID_result.funct3)
			{
				funct3_case(0, SUB)
				funct3_case(5, SRA)
				default:
					complain_wrong_inst_fmt();
			}
			break;
		default:
			complain_wrong_inst_fmt();
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
				complain_wrong_inst_fmt();
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
					complain_wrong_inst_fmt();
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
						complain_wrong_inst_fmt();
					else
						ID_result.exact_op = inst_op::SLLI;
					break;
				case 5:
					if (ID_result.imm >> 5 == 0)
						ID_result.exact_op = inst_op::SRLI;
					else if (ID_result.imm >> 5 == 0x20)
						ID_result.exact_op = inst_op::SRAI;
					else
						complain_wrong_inst_fmt();
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
			complain_wrong_inst_fmt();
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
			complain_wrong_inst_fmt();
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
	if (mispredict)
	{
		ID_stall = true;
		ID_pause = false;
		ID_result = ID_NOP;
		ID_result.pc = pc - 4;
		return;
	}
	if (MEM_pause)
	{
		ID_stall = true;
		return;
	}
	if (old_state.ID_pause)
	{
		if (reg_has_pending_write[ID_result.rs1] || reg_has_pending_write[ID_result.rs2])
			// no need to check last_rd because last_rd = 0
			return;
		ID_pause = false;
		ID_stall = false;
	}
	else
		IF_ID_buff = IF_result;
	ID_result.orig = IF_ID_buff.orig;
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
			complain_wrong_inst_fmt();
	}
	ID_pause =
		reg_has_pending_write[ID_result.rs1] ||
		reg_has_pending_write[ID_result.rs2] ||
		(last_rd != 0 &&
		 ((ID_result.rs1 == last_rd) || (ID_result.rs2 == last_rd)));
	if (!ID_pause)
	{
		ID_stall = false;
		ID_result.rs1_val = reg[ID_result.rs1];
		ID_result.rs2_val = reg[ID_result.rs2];
		ID_result.pc = IF_ID_buff.pc;
		ID_result.jumped = IF_ID_buff.jumped;
		last_rd = ID_result.rd;
	}
	else
	{
		ID_stall = true;
		ID_result = ID_NOP;
		ID_result.pc = IF_ID_buff.pc - 4;
		last_rd = 0;
	}
}
#undef ID_bitmask
