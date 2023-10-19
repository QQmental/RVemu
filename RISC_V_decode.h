#pragma once

#include "RISC_V_spec.h"
#include <limits>
namespace nRISC_V_decode
{
inline RISC_V_Instr_t 
Extract_portion_of_instruction(const RISC_V_Instr_t &targ, uint32_t upper_bit_pos, uint32_t lower_bit_pos);

inline RISC_V_Instr_t opcode(const RISC_V_Instr_t &instruction);
inline RISC_V_Instr_t rd(const RISC_V_Instr_t &instruction);
inline RISC_V_Instr_t rs1(const RISC_V_Instr_t &instruction);
inline RISC_V_Instr_t rs2(const RISC_V_Instr_t &instruction);
inline RISC_V_Instr_t funct3(const RISC_V_Instr_t &instruction);
inline RISC_V_Instr_t funct7(const RISC_V_Instr_t &instruction);

inline void Decode_Rtype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction);
inline void Decode_Itype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction);
inline void Decode_Stype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction);
inline void Decode_Btype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction);
inline void Decode_Utype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction);
inline void Decode_Jtype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction);

inline RISC_V_Instr_t 
Extract_portion_of_instruction(const RISC_V_Instr_t &targ, uint32_t upper_bit_pos, uint32_t lower_bit_pos)
{
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

inline void Decode_Rtype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.rd = rd(instruction);
    result.funct3 = funct3(instruction);
    result.rs1 = rs1(instruction);
    result.rs2 = rs2(instruction);
    result.funct7 = funct7(instruction);
}

inline void Decode_Itype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.rd = rd(instruction);
    result.funct3 = funct3(instruction);
    result.rs1 = rs1(instruction);
    result.imm = Extract_portion_of_instruction(instruction, 31, 20);
}

inline void Decode_Stype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.funct3 = funct3(instruction);
    result.rs1 = rs1(instruction);

    auto imm_11_5 = Extract_portion_of_instruction(instruction, 31, 25);
    auto imm_4_0 = Extract_portion_of_instruction(instruction, 11, 7);
    result.imm = (imm_11_5 << 5) | imm_4_0;
}

inline void Decode_Btype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.funct3 = funct3(instruction);
    result.rs1 = rs1(instruction);
    result.rs2 = rs2(instruction);

    auto imm_12 = Extract_portion_of_instruction(instruction, 31, 31);
    auto imm_10_5 = Extract_portion_of_instruction(instruction, 30, 25);
    auto imm_4_1 = Extract_portion_of_instruction(instruction, 11, 8);
    auto imm_11 = Extract_portion_of_instruction(instruction, 7, 7);
    result.imm = (imm_12 << 12) | (imm_10_5 << 5) | (imm_4_1 << 1) | imm_11;
}

inline void Decode_Utype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.rd = rd(instruction);

    auto imm_31_12 = Extract_portion_of_instruction(instruction, 31, 12);
    result.imm = imm_31_12 << 12;
}

inline void Decode_Jtype_instruction(RV64_Instr_component &result, const RISC_V_Instr_t &instruction)
{
    result.opcode = opcode(instruction);
    result.rd = rd(instruction);

    auto imm_20 = Extract_portion_of_instruction(instruction, 31, 31);
    auto imm_10_1 = Extract_portion_of_instruction(instruction, 30, 21);
    auto imm_11 = Extract_portion_of_instruction(instruction, 20, 20);
    auto imm_19_12 = Extract_portion_of_instruction(instruction, 19, 12);
    result.imm = (imm_20 << 20) | (imm_10_1 << 1) | (imm_11 << 11) | (imm_19_12 << 12);
}
};