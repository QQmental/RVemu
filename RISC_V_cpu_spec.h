#pragma once
#include <string>
#include <stdint.h>
#include "util.h"

namespace nRISC_V_cpu_spec
{

struct RV_Instr_component;
struct RV32_Regster_file;
struct RV64_Regster_file;

using RISC_V_double_word_t = uint64_t;
using RISC_V_word_t = uint32_t;
using RISC_V_half_word_t = uint16_t;

using RISC_V_Instr_t = uint32_t;
using RISC_V_Addr_64_t = uint64_t;
using RISC_V_Addr_32_t = uint32_t;

#if defined(XLEN) == 64
using Int_t = int64_t;
using Uint_t = uint64_t;
using RV_reg_file = RV64_Regster_file;
using RISC_V_Addr_t = RISC_V_Addr_64_t;
using RV_int_reg_t = RISC_V_double_word_t;

#elif defined(XLEN) == 32
using Int_t = int32_t;
using Uint_t = uint32_t;
using RV_reg_file = RV32_Regster_file;
using RISC_V_Addr_t = RISC_V_Addr_32_t;
using RV_int_reg_t = RISC_V_word_t;

#elif defined(XLEN) == 128
using Int_t = int128_t;
using Uint_t = uint128_t;
static_assert(0, "XLEN = 128 is not implemented\n");

#else 
using Int_t = int64_t;
using Uint_t = uint64_t;
using RV_reg_file = RV64_Regster_file;
using RISC_V_Addr_t = RISC_V_Addr_64_t;
using RV_int_reg_t = RISC_V_double_word_t;
#endif

struct RV_Instr_component
{
    RISC_V_Instr_t opcode:7,
                   rd:5,
                   rs1:5,
                   rs2:5, 
                   funct3:3,
                   funct7:7,
                   imm;
};

enum gp_reg_abi_name
{
    zero, ra, sp, gp, tp,
    t0, t1, t2,
    s0, s1,
    a0, a1, a2, a3, a4, a5, a6, a7,
    s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
    t3, t4, t5, t6,
    num_gp_regs,
};

struct RV32_Regster_file
{
    // program counter of current instruction
    RISC_V_Addr_t pc;
    
    RV_int_reg_t gp_regs[32];

    enum reg_num : uint32_t
    {
        x0 = 0, x1 = 1, x2, x3, x4, x5, x6, x7,
        x8, x9, x10, x11, x12, x13, x14, x15,
        x16, x17, x18, x19, x20, x21, x23, 
        x24, x25, x26, x27, x28, x29, x30, x31
    };

    enum freg_num : uint32_t
    {
        f0 = 0, f1 = 1, f2, f3, f4, f5, f6, f7,
        f8, f9, f10, f11, f12, f13, f14, f15,
        f16, f17, f18, f19, f20, f21, f23, 
        f24, f25, f26, f27, f28, f29, f30, f31
    };
};

struct RV64_Regster_file
{
    // program counter of current instruction    
    RISC_V_Addr_t pc;

    RV_int_reg_t gp_regs[32];

    enum reg_num : uint32_t
    {
        x0 = 0, x1 = 1, x2, x3, x4, x5, x6, x7,
        x8, x9, x10, x11, x12, x13, x14, x15,
        x16, x17, x18, x19, x20, x21, x23, 
        x24, x25, x26, x27, x28, x29, x30, x31
    };

    enum freg_num : uint32_t
    {
        f0 = 0, f1 = 1, f2, f3, f4, f5, f6, f7,
        f8, f9, f10, f11, f12, f13, f14, f15,
        f16, f17, f18, f19, f20, f21, f23, 
        f24, f25, f26, f27, f28, f29, f30, f31
    };
};

// pair.first is set 'true' when the attribute is indicated in the ELF
struct RISC_V_Attributes
{
    std::pair<bool, uint32_t>    Tag_RISCV_stack_align;
    std::pair<bool, std::string> Tag_RISCV_arch;
    std::pair<bool, uint32_t>    Tag_RISCV_unaligned_access;
    std::pair<bool, uint32_t>    Tag_RISCV_atomic_abi;
    std::pair<bool, uint32_t>    Tag_RISCV_x3_reg_usage;                
};

struct CPU_Attribute
{
    RISC_V_Attributes RISC_V_attributes;
    nUtil::eEndian endian;
    uint32_t xlen;
};
}

