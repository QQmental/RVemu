#pragma once
#include <limits>
#include "RISC_V_cpu_spec.h"

class BUS;




namespace nRISC_V_cmd
{
struct Instruction_package;

constexpr uint32_t gXLEN = sizeof(nRISC_V_cpu_spec::Int_t) * std::numeric_limits<uint8_t>::digits;

using instr_cmd_t = void (*)(nRISC_V_cmd::Instruction_package&);

enum execution_exception : uint16_t
{
    none = 0,
    divided_by_zero = 1,
    finish,
    trap
};


struct Exec_component
{
    Exec_component(nRISC_V_cpu_spec::RV_reg_file &in_regs, 
                   BUS &in_bus, 
                   const nRISC_V_cpu_spec::RV_Instr_component &in_component)
    : regs(in_regs), 
      bus(in_bus),
      RV_instr_component(in_component)
    {

    }
    nRISC_V_cpu_spec::RV_reg_file &regs;
    const nRISC_V_cpu_spec::RV_Instr_component &RV_instr_component;
    BUS &bus;
};

struct Instruction_package
{
    Instruction_package(Exec_component &exec_component,
                        nRISC_V_cpu_spec::RISC_V_Addr_t in_next_pc,
                        const nRISC_V_cpu_spec::CPU_Attribute &in_CPU_archietecture)
    : m_exec_component(exec_component),
      regs(exec_component.regs), 
      bus(exec_component.bus),
      CPU_attribute(in_CPU_archietecture),
      RV_instr_component(exec_component.RV_instr_component),
      next_pc(in_next_pc),
      except(execution_exception::none)
    {

    }

private:
  Exec_component &m_exec_component;

public:
    nRISC_V_cpu_spec::RV_reg_file &regs;
    BUS &bus;
    const nRISC_V_cpu_spec::CPU_Attribute &CPU_attribute;
    const nRISC_V_cpu_spec::RV_Instr_component &RV_instr_component;
    nRISC_V_cpu_spec::RISC_V_Addr_t next_pc;
    execution_exception except;
};

// ref: riscv-spec-20191213

void ADDI(Instruction_package &instr_pkg);
void ADD(Instruction_package &instr_pkg);
void SUB(Instruction_package &instr_pkg);
void SLT(Instruction_package &instr_pkg);
void SLTU(Instruction_package &instr_pkg);
void SLTI(Instruction_package &instr_pkg);
void SLTIU(Instruction_package &instr_pkg);

void SLLI(Instruction_package &instr_pkg);
void SRLI(Instruction_package &instr_pkg);
void SRAI(Instruction_package &instr_pkg);
void SLL(Instruction_package &instr_pkg);
void SRL(Instruction_package &instr_pkg);
void SRA(Instruction_package &instr_pkg);

void LUI(Instruction_package &instr_pkg);

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

void SYSTEM(Instruction_package &instr_pkg);
void ECALL(Instruction_package &instr_pkg);
void EBREAK(Instruction_package &instr_pkg);

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

void Illegal_CMD(Instruction_package &instr_pkg);
void NOP(Instruction_package &instr_pkg);
void HINT(Instruction_package &instr_pkg);

void C_JAL(Instruction_package &instr_pkg);

/* 
  C.JALR expands to jalr x1, 0(rs1). 
  Strictly speaking, C.JALR does not expand exactly to a base RVI instruction as the value added
  to the PC to form the link address is 2 rather than 4 as in the base ISA,
*/
void C_JALR(Instruction_package &instr_pkg);



}

