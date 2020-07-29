# RISC-V-Simulator
A naive simulator for RISC-V CPU in C++.

Project code for [PPCA 2020](https://acm.sjtu.edu.cn/wiki/PPCA_2020)

Implement RV32I and RV32M instructions.

FENCE will be ignored. For ECALL/EBREAK, behavior is as below.

|reg `a0` (`x10`)| behavior |
|:----------:|:--------|
| 0 | `exit(a1 & 0xff)` |
| 1 | read `int32_t` from standard input and store it into `a1` |
| 2 | print `a1` as `signed int32_t` to standard output |
| 3 | read `unsigned char` from standard input and store it into `a1` |
| 4 | print `a1` as `unsigned char` to standard output |


Contain 5-stage pipeline and branch target prediction.
