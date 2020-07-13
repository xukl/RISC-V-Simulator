#ifndef DEBUG_HPP
#define DEBUG_HPP
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdint>
#include "inst.hpp"
extern uint32_t pc;
extern uint32_t reg[32];
extern bool reg_has_pending_write[32];
extern uint8_t memory[];
extern IF_inst IF_result;
extern ID_inst ID_result;
extern EX_inst EX_result;
extern MEM_inst MEM_result;
extern bool IF_stall, ID_stall, EX_stall, MEM_stall;

struct print_hex32bit
{
	const uint32_t x;
	print_hex32bit(const uint32_t &_x) : x(_x) {}
};
std::ostream &operator<< (std::ostream &os, const print_hex32bit &p)
{
	os << "0x" << std::setw(8) << std::setfill('0') << std::hex << p.x;
	return os;
}
void print_inst_op(inst_op exact_op)
{
	switch (exact_op)
	{
#define op_case(op)\
		case inst_op::op:\
			std::clog << #op;\
			break;
op_case(ADD)op_case(SLT)op_case(SLTU)op_case(AND)op_case(OR)op_case(XOR)op_case(SLL)op_case(SRL)op_case(SUB)op_case(SRA)op_case(ADDI)op_case(SLTI)op_case(SLTIU)op_case(ANDI)op_case(ORI)op_case(XORI)op_case(SLLI)op_case(SRLI)op_case(SRAI)op_case(LB)op_case(LH)op_case(LW)op_case(LBU)op_case(LHU)op_case(SB)op_case(SH)op_case(SW)op_case(BEQ)op_case(BNE)op_case(BLT)op_case(BLTU)op_case(BGE)op_case(BGEU)op_case(LUI)op_case(AUIPC)op_case(JAL)op_case(JALR)
#undef op_case
	}
}
void print_inst_opcode(inst_opcode opcode)
{
	switch (opcode)
	{
#define opcode_case(op)\
		case inst_opcode::op:\
			std::clog << #op;\
			break;
opcode_case(OP_IMM)opcode_case(LOAD)opcode_case(LUI)opcode_case(AUIPC)opcode_case(OP)opcode_case(JAL)opcode_case(JALR)opcode_case(BRANCH)opcode_case(STORE)
#undef op_case
	}
}
void print_ID_detail(const ID_inst &inst)
{
	bool first_print = true;
	switch (inst.format)
	{
#define print_rs1()\
		{\
			if (first_print)\
				first_print = false;\
			else\
				std::clog << ", ";\
			std::clog << "rs1=" << std::setfill('0') << std::setw(2) << int(inst.rs1);\
		}
#define print_rs2()\
		{\
			if (first_print)\
				first_print = false;\
			else\
				std::clog << ", ";\
			std::clog << "rs2=" << std::setfill('0') << std::setw(2) << int(inst.rs2);\
		}
#define print_rd()\
		{\
			if (first_print)\
				first_print = false;\
			else\
				std::clog << ", ";\
			std::clog << "rd=" << std::setfill('0') << std::setw(2) << int(inst.rd);\
		}
#define print_imm()\
		{\
			if (first_print)\
				first_print = false;\
			else\
				std::clog << ", ";\
			std::clog << "imm=" << print_hex32bit(inst.imm);\
		}
		case inst_format::R:
			print_rs1();
			print_rs2();
			print_rd();
			break;
		case inst_format::I:
			print_rs1();
			print_rd();
			print_imm();
			break;
		case inst_format::S:
			print_rs1();
			print_rs2();
			print_imm();
			break;
		case inst_format::B:
			print_rs1();
			print_rs2();
			print_rd();
			break;
		case inst_format::U:
			print_rd();
			print_imm();
			break;
		case inst_format::J:
			print_rd();
			print_imm();
			break;
#undef print_rs1
#undef print_rs2
#undef print_rd
#undef print_imm
	}
}
void print_reg()
{
	std::clog << "pc : " << print_hex32bit(pc) << std::endl;
	for (int i = 0; i < 32; ++i)
	{
		std::clog << 'x' << std::setw(2) << i << ": " << print_hex32bit(reg[i]);
		if ((i + 1) % 4 == 0)
			std::clog << std::endl;
		else
			std::clog << "  ";
	}
	std::clog << std::endl;
}
void print_pipeline()
{
	// IF
	std::clog << "IF:  (" << (IF_stall ? 'X' : 'O') << ")  pc: " << print_hex32bit(IF_result.pc) << "  raw: " << print_hex32bit(IF_result.orig) << std::endl;

	// ID
	std::clog << "ID:  (" << (ID_stall ? 'X' : 'O') << ")  pc: " << print_hex32bit(ID_result.pc) << "  raw: " << print_hex32bit(ID_result.orig) << "  op: ";
	if (ID_result.orig == ID_NOP.orig)
		std::clog << "NOP";
	else
		print_inst_op(ID_result.exact_op);
	std::clog << std::endl;

	// EX
	std::clog << "EX:  (" << (EX_stall ? 'X' : 'O') << ")  op: ";
	if (EX_result.opcode == inst_opcode::OP_IMM && EX_result.val == 0 && EX_result.reg == 0)
		std::clog << "NOP";
	else
		print_inst_opcode(EX_result.opcode);
	std::clog << std::endl;

	// MEM
	std::clog << "MEM: (" << (MEM_stall ? 'X' : 'O') << ")  op: ";
	if (MEM_result.opcode == inst_opcode::OP_IMM && MEM_result.val == 0 && MEM_result.reg == 0)
		std::clog << "NOP";
	else
		print_inst_opcode(MEM_result.opcode);
	std::clog << std::endl;

	std::clog << std::endl;
}
#endif
