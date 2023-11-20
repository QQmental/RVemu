#pragma once
#include <string>
#include <stdint.h>
#include "util.h"

namespace nRISC_V_cpu_spec
{

struct RV_Instr_component;
struct RV32I_regster_file;
struct RV64I_regster_file;

using RISC_V_double_word_t = uint64_t;
using RISC_V_word_t = uint32_t;
using RISC_V_half_word_t = uint16_t;

using RISC_V_Instr_t = uint32_t;
using RISC_V_Addr_64_t = uint64_t;
using RISC_V_Addr_32_t = uint32_t;

#if defined(XLEN) == 64
using Int_t = int64_t;
using Uint_t = uint64_t;
using RV_reg_file = RV64I_regster_file;
using RISC_V_Addr_t = RISC_V_Addr_64_t;

#elif defined(XLEN) == 32
using Int_t = int32_t;
using Uint_t = uint32_t;
using RV_reg_file = RV32I_regster_file;
using RISC_V_Addr_t = RISC_V_Addr_32_t;

#elif defined(XLEN) == 128
using Int_t = int128_t;
using Uint_t = uint128_t;
static_assert(0, "XLEN = 128 is not implemented\n");

#else 
using Int_t = int64_t;
using Uint_t = uint64_t;
using RV_reg_file = RV64I_regster_file;
using RISC_V_Addr_t = RISC_V_Addr_64_t;
#endif

struct RV_Instr_component
{
    RISC_V_Instr_t opcode, rd, rs1, rs2, funct3, funct7, imm;
};

struct RV32I_regster_file
{
    RISC_V_word_t pc;
    
    RISC_V_word_t gp_regs[32];

    enum reg_num : uint32_t
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

    enum reg_num : uint32_t
    {
        x0 = 0, x1 = 1, x2, x3, x4, x5, x6, x7,
        x8, x9, x10, x11, x12, x13, x14, x15,
        x16, x17, x18, x19, x20, x21, x23, 
        x24, x25, x26, x27, x28, x29, x30, x31
    };
};

struct RISC_V_Attributes
{
    // first of a atrribute is set 'true' when that attribute is indicated in the ELF
    std::pair<bool, uint32_t>    Tag_RISCV_stack_align;
    std::pair<bool, std::string> Tag_RISCV_arch;
    std::pair<bool, uint32_t>    Tag_RISCV_unaligned_access;
    std::pair<bool, uint32_t>    Tag_RISCV_atomic_abi;
    std::pair<bool, uint32_t>    Tag_RISCV_x3_reg_usage;                
};

struct CPU_Attribute
{
    RISC_V_Attributes RISC_V_attributes;

//  address allowed accessed between [base_addr, highest_addr)
    RISC_V_Addr_t highest_addr;
    RISC_V_Addr_t base_addr;
    nUtil::eEndian endian;
    uint32_t xlen;
};
}

