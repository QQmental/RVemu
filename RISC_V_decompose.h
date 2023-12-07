#pragma once

#include "RISC_V_cpu_spec.h"
#include <limits>
namespace nRISC_V_decompose
{

/*
non-compressed instruction format
31       30 29 28 27 26 25 24 23 22 21   20      19 18 17 16 15 14 13 12 11 10 9   8           7 6 5 4 3 2 1 0
            funct7        |   rs2               |     rs1      | funct3 |        rd             |    opcode   | R-type
                  imm[11:0]                     |     rs1      | funct3 |        rd             |    opcode   | I-type
           imm[11:5]      |   rs2               |     rs1      | funct3 |  imm[4:0]             |    opcode   | S-type
imm[12] |     imm[10:5]   |   rs2               |     rs1      | funct3 || imm[4:1]   imm[11]   |    opcode   | B-type
                           imm[31:12]                                   |        rd             |    opcode   | U-type
imm[20] |     imm[10:1]               | imm[11] |     imm[19:12]        |        rd             |    opcode   | J-type





*/


/*
a type RISC_V_Instr_t varriable can represent either a non-compressed or compressed instruction

*/

using namespace nRISC_V_cpu_spec;

enum eOpecode_type
{
    R = 0,
    I,
    S,
    B,
    U,
    J
};

struct Opcode_type_match 
{
    RISC_V_Instr_t opcode;
    eOpecode_type type;
};

constexpr Opcode_type_match  gOPCODE_MATCH_TABLE[] = 
{
    {0b0110011, eOpecode_type::R},
    {0b0000011, eOpecode_type::I},
    {0b0010011, eOpecode_type::I},
    {0b0011011, eOpecode_type::I},
    {0b0001111, eOpecode_type::I},
    {0b0100011, eOpecode_type::S},
    {0b1100011, eOpecode_type::B},
    {0b0110111, eOpecode_type::U},
    {0b0010111, eOpecode_type::U},
    {0b1101111, eOpecode_type::J},
    {0b0111011, eOpecode_type::R}
};

inline RISC_V_Instr_t 
Extract_portion_of_instruction(const RISC_V_Instr_t &targ, uint32_t upper_bit_pos, uint32_t lower_bit_pos);

inline RISC_V_Instr_t opcode(const RISC_V_Instr_t &instruction);
inline RISC_V_Instr_t rd(const RISC_V_Instr_t &instruction);
inline RISC_V_Instr_t rs1(const RISC_V_Instr_t &instruction);
inline RISC_V_Instr_t rs2(const RISC_V_Instr_t &instruction);
inline RISC_V_Instr_t funct3(const RISC_V_Instr_t &instruction);
inline RISC_V_Instr_t funct7(const RISC_V_Instr_t &instruction);

inline void Decompose_Rtype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction);
inline void Decompose_Itype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction);
inline void Decompose_Stype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction);
inline void Decompose_Btype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction);
inline void Decompose_Utype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction);
inline void Decompose_Jtype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction);

inline RISC_V_Instr_t 
Extract_portion_of_instruction(const RISC_V_Instr_t &targ, uint32_t upper_bit_pos, uint32_t lower_bit_pos)
{
    assert(upper_bit_pos >= lower_bit_pos);

    auto left_shift_len = sizeof(RISC_V_Instr_t) * std::numeric_limits<uint8_t>::digits - 1 - upper_bit_pos;
    auto t = targ << left_shift_len;
    return t >> (sizeof(RISC_V_Instr_t) * std::numeric_limits<uint8_t>::digits - 1 - upper_bit_pos + lower_bit_pos);
}

inline RISC_V_Instr_t opcode(const RISC_V_Instr_t &instruction)
{
    return Extract_portion_of_instruction(instruction, 6, 0);   
}

inline RISC_V_Instr_t rd(const RISC_V_Instr_t &instruction)
{
    return Extract_portion_of_instruction(instruction, 11, 7); 
}
inline RISC_V_Instr_t rs1(const RISC_V_Instr_t &instruction)
{
    return Extract_portion_of_instruction(instruction, 19, 15);     
}
inline RISC_V_Instr_t rs2(const RISC_V_Instr_t &instruction)
{
    return Extract_portion_of_instruction(instruction, 24, 20);     
}
inline RISC_V_Instr_t funct3(const RISC_V_Instr_t &instruction)
{
    return Extract_portion_of_instruction(instruction, 14, 12);    
}
inline RISC_V_Instr_t funct7(const RISC_V_Instr_t &instruction)
{
    return Extract_portion_of_instruction(instruction, 31, 25);    
}

inline void Decompose_Rtype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.rd = rd(instruction);
    result.funct3 = funct3(instruction);
    result.rs1 = rs1(instruction);
    result.rs2 = rs2(instruction);
    result.funct7 = funct7(instruction);
}

inline void Decompose_Itype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.rd = rd(instruction);
    result.funct3 = funct3(instruction);
    result.rs1 = rs1(instruction);
    result.imm = Extract_portion_of_instruction(instruction, 31, 20);
}

inline void Decompose_Stype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.funct3 = funct3(instruction);
    result.rs1 = rs1(instruction);
    result.rs2 = rs2(instruction);
    
    auto imm_11_5 = Extract_portion_of_instruction(instruction, 31, 25);
    auto imm_4_0 = Extract_portion_of_instruction(instruction, 11, 7);
    result.imm = (imm_11_5 << 5) | imm_4_0;
}

inline void Decompose_Btype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.funct3 = funct3(instruction);
    result.rs1 = rs1(instruction);
    result.rs2 = rs2(instruction);

    auto imm_12 = Extract_portion_of_instruction(instruction, 31, 31);
    auto imm_10_5 = Extract_portion_of_instruction(instruction, 30, 25);
    auto imm_4_1 = Extract_portion_of_instruction(instruction, 11, 8);
    auto imm_11 = Extract_portion_of_instruction(instruction, 7, 7);
    result.imm = (imm_12 << 12) | (imm_10_5 << 5) | (imm_4_1 << 1) | (imm_11 << 11);
}

inline void Decompose_Utype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.rd = rd(instruction);

    auto imm_31_12 = Extract_portion_of_instruction(instruction, 31, 12);
    result.imm = imm_31_12 << 12;
}

inline void Decompose_Jtype_instruction(RV_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.rd = rd(instruction);

    auto imm_20 = Extract_portion_of_instruction(instruction, 31, 31);
    auto imm_10_1 = Extract_portion_of_instruction(instruction, 30, 21);
    auto imm_11 = Extract_portion_of_instruction(instruction, 20, 20);
    auto imm_19_12 = Extract_portion_of_instruction(instruction, 19, 12);
    result.imm = (imm_20 << 20) | (imm_10_1 << 1) | (imm_11 << 11) | (imm_19_12 << 12);
}
}