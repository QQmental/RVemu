#pragma once

#include <stdint.h>

using RISC_V_double_word_t = uint64_t;
using RISC_V_word_t = uint32_t;
using RISC_V_half_word_t = uint16_t;

using RISC_V_Instr_t = uint32_t;

struct RV64_Instr_component
{
    RISC_V_Instr_t opcode, rd, rs1, rs2, funct3, funct7, imm;
};

struct RV32I_regster_file
{
    RISC_V_word_t pc;
    
    RISC_V_word_t gp_regs[32];

    enum reg_abi : uint32_t
    {
        x0 = 0, x1 = 1, x2, x3, x4, x5, x6, x7,
        x8, x9, x10, x11, x12, x13, x14, x15,
        x16, x17, x18, x19, x20, x21, x23, 
        x24, x25, x26, x27, x28, x29, x30, x31
    };
};

struct RV64I_regster_file
{
    RISC_V_word_t pc;

    RISC_V_double_word_t gp_regs[32];

    enum reg_abi : uint32_t
    {
        x0 = 0, x1 = 1, x2, x3, x4, x5, x6, x7,
        x8, x9, x10, x11, x12, x13, x14, x15,
        x16, x17, x18, x19, x20, x21, x23, 
        x24, x25, x26, x27, x28, x29, x30, x31
    };
};

struct RISC_V_Attributes
{
    uint32_t Tag_RISCV_stack_align;
    std::string Tag_RISCV_arch;
    uint32_t Tag_RISCV_unaligned_access;
    uint32_t Tag_RISCV_atomic_abi;
    uint32_t Tag_RISCV_x3_reg_usage;
};