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
extern uint32_t IF_result;
extern ID_inst ID_result;
extern EX_inst EX_result;
extern MEM_inst MEM_result;
extern bool ID_stall, EX_stall, MEM_stall;

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
void print_ID_result()
{
	std::cout << "orig: " << print_hex32bit(ID_result.orig) << std::endl;
	std::cout << "operation: ";
	switch (ID_result.exact_op)
	{
#define op_case(op)\
		case inst_op::op:\
			std::cout << #op;\
			break;
op_case(ADD)op_case(SLT)op_case(SLTU)op_case(AND)op_case(OR)op_case(XOR)op_case(SLL)op_case(SRL)op_case(SUB)op_case(SRA)op_case(ADDI)op_case(SLTI)op_case(SLTIU)op_case(ANDI)op_case(ORI)op_case(XORI)op_case(SLLI)op_case(SRLI)op_case(SRAI)op_case(LB)op_case(LH)op_case(LW)op_case(LBU)op_case(LHU)op_case(SB)op_case(SH)op_case(SW)op_case(BEQ)op_case(BNE)op_case(BLT)op_case(BLTU)op_case(BGE)op_case(BGEU)op_case(LUI)op_case(AUIPC)op_case(JAL)op_case(JALR)
	}
	switch (ID_result.format)
	{
#define print_rs1()\
		{\
			std::cout << ", rs1=" << std::setfill('0') << std::setw(2) << int(ID_result.rs1);\
		}
#define print_rs2()\
		{\
			std::cout << ", rs2=" << std::setfill('0') << std::setw(2) << int(ID_result.rs2);\
		}
#define print_rd()\
		{\
			std::cout << ", rd=" << std::setfill('0') << std::setw(2) << int(ID_result.rd);\
		}
#define print_imm()\
		{\
			std::cout << ", imm=" << print_hex32bit(ID_result.imm);\
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
	std::cout << std::endl;
}
void print_stall()
{
	std::cout << "ID (" << (ID_stall ? 'x' : 'o') << ")  ";
	std::cout << "EX (" << (EX_stall ? 'x' : 'o') << ")  ";
	std::cout << "MEM (" << (MEM_stall ? 'x' : 'o') << ")\n" << std::endl;
}
void print_reg()
{
	std::cout << "pc : " << print_hex32bit(pc) << std::endl;
	for (int i = 0; i < 32; ++i)
	{
		std::cout << 'x' << std::setw(2) << std::dec << i << ": " << print_hex32bit(reg[i]) << "  ";
		if ((i + 1) % 4 == 0)
			std::cout << std::endl;
	}
	std::cout << std::endl;
}
#endif
