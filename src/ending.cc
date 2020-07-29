#include <iostream>
#include <cstdlib>
#include <cstdint>
#include "state.hpp"
extern const volatile state old_state;
const volatile uint32_t *const reg = old_state.reg;
const extern int BP_cnt_fail, BP_cnt_success;
void end_of_simulation(int exit_val)
{
	std::cout << std::dec << (exit_val & 0xff) << std::endl;
	std::clog << "branch prediction success rate: ";
	std::clog.precision(10);
	std::clog << double(BP_cnt_success) / (BP_cnt_success + BP_cnt_fail) << std::endl;
	exit(0);
}
