#pragma once
#include <limits>
#include "RISC_V_cpu_spec.h"

class BUS;

namespace nRISC_V_cmd
{

/* default Xlength = 64 */

#if defined(XLEN) == 64
using Int_t = int64_t;
using Uint_t = uint64_t;
using RV_reg_file = RV64I_regster_file;

#elif defined(XLEN) == 32
using Int_t = int32_t;
using Uint_t = uint32_t;
using RV_reg_file = RV32I_regster_file;

#elif defined(XLEN) == 128
using Int_t = int128_t;
using Uint_t = uint128_t;

static_assert(0, "XLEN = 128 is not implemented\n");

#else 
using Int_t = int64_t;
using Uint_t = uint64_t;
using RV_reg_file = RV64I_regster_file;
#endif

constexpr uint32_t gXLEN = sizeof(Int_t) * std::numeric_limits<uint8_t>::digits;


enum execution_exception : uint16_t
{
    none = 0,
    divided_by_zero = 1,
    finish
};



struct Instruction_package
{
    Instruction_package(RV_reg_file &in_regs, 
                        BUS &in_bus, 
                        const RV_Instr_component &in_component, 
                        const CPU_Attribute &in_CPU_archietecture,
                        const RISC_V_Instr_t &in_instruction)
    : regs(in_regs), 
      bus(in_bus),
      CPU_attribute(in_CPU_archietecture),
      RV_instr_component(in_component),
      instruction(in_instruction), 
      except(execution_exception::none)
    {

    }
    RV_reg_file &regs;
    BUS &bus;
    const CPU_Attribute &CPU_attribute;
    const RV_Instr_component &RV_instr_component;
    RISC_V_Instr_t instruction;
    execution_exception except; 
};

void ADDI(Instruction_package &instr_pkg);
void ADD(Instruction_package &instr_pkg);
void SUB(Instruction_package &instr_pkg);
void SLT(Instruction_package &instr_pkg);
void SLTU(Instruction_package &instr_pkg);
void SLTI(Instruction_package &instr_pkg);
void SLTIU(Instruction_package &instr_pkg);

void RV32_SLLI(Instruction_package &instr_pkg);
void RV32_SRLI(Instruction_package &instr_pkg);
void RV32_SRAI(Instruction_package &instr_pkg);
void RV32_SLL(Instruction_package &instr_pkg);
void RV32_SRL(Instruction_package &instr_pkg);
void RV32_SRA(Instruction_package &instr_pkg);

void RV64_SLLI(Instruction_package &instr_pkg);
void RV64_SRLI(Instruction_package &instr_pkg);
void RV64_SRAI(Instruction_package &instr_pkg);
void RV64_SLL(Instruction_package &instr_pkg);
void RV64_SRL(Instruction_package &instr_pkg);
void RV64_SRA(Instruction_package &instr_pkg);

void RV32_LUI(Instruction_package &instr_pkg);
void RV64_LUI(Instruction_package &instr_pkg);

void AUIPC(Instruction_package &instr_pkg);

void XORI(Instruction_package &instr_pkg);
void ORI(Instruction_package &instr_pkg);
void ANDI(Instruction_package &instr_pkg);
void XOR(Instruction_package &instr_pkg);
void OR(Instruction_package &instr_pkg);
void AND(Instruction_package &instr_pkg);
void LB(Instruction_package &instr_pkg);
void LH(Instruction_package &instr_pkg);
void LW(Instruction_package &instr_pkg);
void LD(Instruction_package &instr_pkg);
void LBU(Instruction_package &instr_pkg);
void LHU(Instruction_package &instr_pkg);
void LWU(Instruction_package &instr_pkg);

void SB(Instruction_package &instr_pkg);
void SH(Instruction_package &instr_pkg);
void SW(Instruction_package &instr_pkg);
void SD(Instruction_package &instr_pkg);

void BEQ(Instruction_package &instr_pkg);
void BNE(Instruction_package &instr_pkg);
void BLT(Instruction_package &instr_pkg);
void BGE(Instruction_package &instr_pkg);
void BLTU(Instruction_package &instr_pkg);
void BGEQ(Instruction_package &instr_pkg);
void JAl(Instruction_package &instr_pkg);
void JALR(Instruction_package &instr_pkg);

void E_CALL_or_BREAK(Instruction_package &instr_pkg);

void FENCE_I(Instruction_package &instr_pkg);

void CSRRW(Instruction_package &instr_pkg);
void CSRRS(Instruction_package &instr_pkg);
void CSRRC(Instruction_package &instr_pkg);
void CSRRWI(Instruction_package &instr_pkg);
void CSRRSI(Instruction_package &instr_pkg);
void CSRRCI(Instruction_package &instr_pkg);

void ADDIW(Instruction_package &instr_pkg);
void SLLIW(Instruction_package &instr_pkg);
void SRLIW(Instruction_package &instr_pkg);
void SRAIW(Instruction_package &instr_pkg);

void ADDW(Instruction_package &instr_pkg);
void SLLW(Instruction_package &instr_pkg);
void SRLW(Instruction_package &instr_pkg);
void SRAW(Instruction_package &instr_pkg);
void SUBW(Instruction_package &instr_pkg);

}

