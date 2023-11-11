#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <limits>

#include "RISC_V_cpu_spec.h"
#include "RISC_V_decompose.h"
#include "RISC_V_cmd.h"
#include "BUS.h"
#include "util.h"

using namespace nRISC_V_cmd;

template <typename ret_Int_t, unsigned B>
ret_Int_t Signed_extend(RISC_V_double_word_t num);


void nRISC_V_cmd::ADDI(Instruction_package &instr_pkg);
void nRISC_V_cmd::ADD(Instruction_package &instr_pkg);
void nRISC_V_cmd::SUB(Instruction_package &instr_pkg);
void nRISC_V_cmd::SLT(Instruction_package &instr_pkg);
void nRISC_V_cmd::SLTU(Instruction_package &instr_pkg);
void nRISC_V_cmd::SLTI(Instruction_package &instr_pkg);
void nRISC_V_cmd::SLTIU(Instruction_package &instr_pkg);

void nRISC_V_cmd::RV32_SLLI(Instruction_package &instr_pkg);
void nRISC_V_cmd::RV32_SRLI(Instruction_package &instr_pkg);
void nRISC_V_cmd::RV32_SRAI(Instruction_package &instr_pkg);
void nRISC_V_cmd::RV32_SLL(Instruction_package &instr_pkg);
void nRISC_V_cmd::RV32_SRL(Instruction_package &instr_pkg);
void nRISC_V_cmd::RV32_SRA(Instruction_package &instr_pkg);

void nRISC_V_cmd::RV64_SLLI(Instruction_package &instr_pkg);
void nRISC_V_cmd::RV64_SRLI(Instruction_package &instr_pkg);
void nRISC_V_cmd::RV64_SRAI(Instruction_package &instr_pkg);
void nRISC_V_cmd::RV64_SLL(Instruction_package &instr_pkg);
void nRISC_V_cmd::RV64_SRL(Instruction_package &instr_pkg);
void nRISC_V_cmd::RV64_SRA(Instruction_package &instr_pkg);

void nRISC_V_cmd::RV32_LUI(Instruction_package &instr_pkg);
void nRISC_V_cmd::RV64_LUI(Instruction_package &instr_pkg);

void nRISC_V_cmd::XORI(Instruction_package &instr_pkg);
void nRISC_V_cmd::ORI(Instruction_package &instr_pkg);
void nRISC_V_cmd::ANDI(Instruction_package &instr_pkg);
void nRISC_V_cmd::XOR(Instruction_package &instr_pkg);
void nRISC_V_cmd::OR(Instruction_package &instr_pkg);
void nRISC_V_cmd::AND(Instruction_package &instr_pkg);
void nRISC_V_cmd::LB(Instruction_package &instr_pkg);
void nRISC_V_cmd::LH(Instruction_package &instr_pkg);
void nRISC_V_cmd::LW(Instruction_package &instr_pkg);
void nRISC_V_cmd::LBU(Instruction_package &instr_pkg);
void nRISC_V_cmd::LHU(Instruction_package &instr_pkg);
void nRISC_V_cmd::SB(Instruction_package &instr_pkg);
void nRISC_V_cmd::SH(Instruction_package &instr_pkg);
void nRISC_V_cmd::SW(Instruction_package &instr_pkg);
void nRISC_V_cmd::BEQ(Instruction_package &instr_pkg);
void nRISC_V_cmd::BNE(Instruction_package &instr_pkg);
void nRISC_V_cmd::BLT(Instruction_package &instr_pkg);
void nRISC_V_cmd::BGE(Instruction_package &instr_pkg);
void nRISC_V_cmd::BLTU(Instruction_package &instr_pkg);
void nRISC_V_cmd::BGEQ(Instruction_package &instr_pkg);
void nRISC_V_cmd::JAl(Instruction_package &instr_pkg);
void nRISC_V_cmd::JALR(Instruction_package &instr_pkg);

void nRISC_V_cmd::E_CALL_or_BREAK(Instruction_package &instr_pkg);
static void ECALL_impl(Instruction_package &instr_pkg);
static void EBREAK_impl(Instruction_package &instr_pkg);

void nRISC_V_cmd::FENCE_I(Instruction_package &instr_pkg);

void nRISC_V_cmd::CSRRW(Instruction_package &instr_pkg);
void nRISC_V_cmd::CSRRS(Instruction_package &instr_pkg);
void nRISC_V_cmd::CSRRC(Instruction_package &instr_pkg);
void nRISC_V_cmd::CSRRWI(Instruction_package &instr_pkg);
void nRISC_V_cmd::CSRRSI(Instruction_package &instr_pkg);
void nRISC_V_cmd::CSRRCI(Instruction_package &instr_pkg);

void nRISC_V_cmd::ADDIW(Instruction_package &instr_pkg);
void nRISC_V_cmd::SLLIW(Instruction_package &instr_pkg);
void nRISC_V_cmd::SRLIW(Instruction_package &instr_pkg);
void nRISC_V_cmd::SRAIW(Instruction_package &instr_pkg);
void nRISC_V_cmd::ADDW(Instruction_package &instr_pkg);
void nRISC_V_cmd::SLLW(Instruction_package &instr_pkg);
void nRISC_V_cmd::SRLW(Instruction_package &instr_pkg);
void nRISC_V_cmd::SRAW(Instruction_package &instr_pkg);
void nRISC_V_cmd::SUBW(Instruction_package &instr_pkg);

template <typename ret_Int_t, unsigned B>
ret_Int_t Signed_extend(RISC_V_double_word_t num)
{
    struct {ret_Int_t x:B;} s;
    
    s.x = num;
    
    return s.x;
}

void nRISC_V_cmd::ADDI(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(static_cast<RISC_V_word_t>(instr_pkg.RV_instr_component.imm));

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];
    
    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];
    
    rd_val = se_imm + static_cast<Int_t>(rs1_val);
}

void nRISC_V_cmd::ADD(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = static_cast<Int_t>(rs1_val) + static_cast<Int_t>(rs2_val);
}

void nRISC_V_cmd::SUB(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = static_cast<Int_t>(rs1_val) - static_cast<Int_t>(rs2_val);
}

void nRISC_V_cmd::SLT(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if (static_cast<Int_t>(rs1_val) < static_cast<Int_t>(rs2_val))
        rd_val = 1;
    else
        rd_val = 0;
}

void nRISC_V_cmd::SLTU(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if (static_cast<Uint_t>(rs1_val) < static_cast<Uint_t>(rs2_val))
        rd_val = 1;
    else
        rd_val = 0;
}

void nRISC_V_cmd::SLTI(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(static_cast<RISC_V_word_t>(instr_pkg.RV_instr_component.imm));

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];
    
    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];
    
    if (static_cast<Int_t>(rs1_val) < se_imm)
        rd_val = 1;
    else
        rd_val = 0;
}

void nRISC_V_cmd::SLTIU(Instruction_package &instr_pkg)
{
    auto imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];
    
    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];
    
    if (static_cast<Uint_t>(rs1_val) < static_cast<Uint_t>(imm))
        rd_val = 1;
    else
        rd_val = 0;
}

void nRISC_V_cmd::RV32_SLLI(Instruction_package &instr_pkg)
{
    auto shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 4, 0);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val << shamt;
}

void nRISC_V_cmd::RV32_SRLI(Instruction_package &instr_pkg)
{
    auto shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 4, 0);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val >> shamt;
}

// shift right arithmetic by immediate
void nRISC_V_cmd::RV32_SRAI(Instruction_package &instr_pkg)
{
    auto shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 4, 0);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = static_cast<Int_t>(rs1_val) >> shamt;
}

void nRISC_V_cmd::RV32_SLL(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val << (rs2_val & 0b11111);
}

void nRISC_V_cmd::RV32_SRL(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val >> (rs2_val & 0b11111);
}

void nRISC_V_cmd::RV32_SRA(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = static_cast<Int_t>(rs1_val) >> (rs2_val & 0b11111);
}

void nRISC_V_cmd::RV64_SLLI(Instruction_package &instr_pkg)
{
    auto shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 5, 0);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val << shamt;
}

void nRISC_V_cmd::RV64_SRLI(Instruction_package &instr_pkg)
{
    auto shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 5, 0);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val >> shamt;
}

// shift right arithmetic by immediate
void nRISC_V_cmd::RV64_SRAI(Instruction_package &instr_pkg)
{
    auto shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 5, 0);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = static_cast<Int_t>(rs1_val) >> shamt;
}

void nRISC_V_cmd::RV64_SLL(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rd_val << (rs1_val & RISC_V_double_word_t(0b111111));
}

void nRISC_V_cmd::RV64_SRL(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rd_val >> (rs1_val & RISC_V_double_word_t(0b111111));
}

void nRISC_V_cmd::RV64_SRA(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = static_cast<Int_t>(rd_val) >> (rs1_val & RISC_V_double_word_t(0b111111));
}

void nRISC_V_cmd::RV32_LUI(Instruction_package &instr_pkg)
{
    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = instr_pkg.RV_instr_component.imm << 12;
}

void nRISC_V_cmd::RV64_LUI(Instruction_package &instr_pkg)
{
    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = instr_pkg.RV_instr_component.imm << 12;

    rd_val = Signed_extend<Int_t, 32>(rd_val);
}

void nRISC_V_cmd::AUIPC(Instruction_package &instr_pkg)
{
    auto imm = Signed_extend<Int_t, 32>(instr_pkg.RV_instr_component.imm << 20);

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = imm + instr_pkg.regs.pc;
}

void nRISC_V_cmd::XORI(Instruction_package &instr_pkg)
{
    auto imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = imm ^ rs1_val;
}

void nRISC_V_cmd::ORI(Instruction_package &instr_pkg)
{
    auto imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = imm | rs1_val;
}

void nRISC_V_cmd::ANDI(Instruction_package &instr_pkg)
{
    auto imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = imm & rs1_val;
}

void nRISC_V_cmd::XOR(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val ^ rs2_val;
}

void nRISC_V_cmd::OR(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val | rs2_val;
}

void nRISC_V_cmd::AND(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val & rs2_val;
}

void nRISC_V_cmd::LB(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    instr_pkg.bus.Load_data(reinterpret_cast<char*>(&rd_val), static_cast<Int_t>(rs1_val) + se_imm, 1);
    
    if (instr_pkg.CPU_attribute.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(rd_val);
    
    rd_val = Signed_extend<Int_t, 8>(rd_val);
}

void nRISC_V_cmd::LH(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    instr_pkg.bus.Load_data(reinterpret_cast<char*>(&rd_val), static_cast<Int_t>(rs1_val) + se_imm, 2);
    
    if (instr_pkg.CPU_attribute.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(rd_val);
    
    rd_val = Signed_extend<Int_t, 16>(rd_val);
}

void nRISC_V_cmd::LW(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    instr_pkg.bus.Load_data(reinterpret_cast<char*>(&rd_val), static_cast<Int_t>(rs1_val) + se_imm, 4);
    
    if (instr_pkg.CPU_attribute.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(rd_val);
    
    rd_val = Signed_extend<Int_t, 32>(rd_val);
}

void nRISC_V_cmd::LD(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    instr_pkg.bus.Load_data(reinterpret_cast<char*>(&rd_val), static_cast<Int_t>(rs1_val) + se_imm, 8);
    
    if (instr_pkg.CPU_attribute.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(rd_val);
}

void nRISC_V_cmd::LBU(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = 0;

    instr_pkg.bus.Load_data(reinterpret_cast<char*>(&rd_val), static_cast<Int_t>(rs1_val) + se_imm, 1);
    
    if (instr_pkg.CPU_attribute.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(rd_val);
}

void nRISC_V_cmd::LHU(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = 0;

    instr_pkg.bus.Load_data(reinterpret_cast<char*>(&rd_val), static_cast<Int_t>(rs1_val) + se_imm, 2);
    
    if (instr_pkg.CPU_attribute.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(rd_val);
}

void nRISC_V_cmd::LWU(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = 0;

    instr_pkg.bus.Load_data(reinterpret_cast<char*>(&rd_val), static_cast<Int_t>(rs1_val) + se_imm, 4);
    
    if (instr_pkg.CPU_attribute.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(rd_val);
}

void nRISC_V_cmd::SB(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    uint8_t val = rs1_val & 0xFF;
    
    if (instr_pkg.CPU_attribute.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(val);
    
    instr_pkg.bus.Store_data(reinterpret_cast<const char*>(&val), static_cast<Int_t>(rs1_val) + se_imm, sizeof(val));
}

void nRISC_V_cmd::SH(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    uint16_t val = rs1_val & 0xFFFF;
    
    if (instr_pkg.CPU_attribute.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(val);
    
    instr_pkg.bus.Store_data(reinterpret_cast<const char*>(&val), static_cast<Int_t>(rs1_val) + se_imm, sizeof(val));
}

void nRISC_V_cmd::SW(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    uint32_t val = rs1_val & 0xFFFF'FFFF;
    
    if (instr_pkg.CPU_attribute.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(val);
    
    instr_pkg.bus.Store_data(reinterpret_cast<const char*>(&val), static_cast<Int_t>(rs1_val) + se_imm, sizeof(val));
}

void nRISC_V_cmd::SD(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    uint64_t val = rs1_val & 0xFFFF'FFFF'FFFF'FFFF;
    
    if (instr_pkg.CPU_attribute.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(val);
    
    instr_pkg.bus.Store_data(reinterpret_cast<const char*>(&val), static_cast<Int_t>(rs1_val) + se_imm, sizeof(val));
}

void nRISC_V_cmd::BEQ(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    if (rs1_val == rs2_val)
        instr_pkg.regs.pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::BNE(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    if (rs1_val != rs2_val)
        instr_pkg.regs.pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::BLT(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    if (static_cast<Int_t>(rs1_val) < static_cast<Int_t>(rs2_val))
        instr_pkg.regs.pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::BGE(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    if (static_cast<Int_t>(rs1_val) >= static_cast<Int_t>(rs2_val))
        instr_pkg.regs.pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::BLTU(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    if (rs1_val < rs2_val)
        instr_pkg.regs.pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::BGEQ(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    if (rs1_val >= rs2_val)
        instr_pkg.regs.pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::JAl(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 20>(instr_pkg.RV_instr_component.imm);
    
    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = static_cast<Int_t>(instr_pkg.regs.pc) + 4;

    instr_pkg.regs.pc = instr_pkg.regs.pc + se_imm * 2;
}

void nRISC_V_cmd::JALR(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = instr_pkg.regs.pc + 4;

    instr_pkg.regs.pc = (instr_pkg.regs.pc + se_imm) & (~0x1);
}

void nRISC_V_cmd::E_CALL_or_BREAK(Instruction_package &instr_pkg)
{
    assert(instr_pkg.RV_instr_component.rd == 0 && instr_pkg.RV_instr_component.rs1 == 0);

    switch (instr_pkg.RV_instr_component.imm)
    {
        case 0:
            ECALL_impl(instr_pkg);
            break;
        case 1:
            EBREAK_impl(instr_pkg);
        break;            
        default:
            printf("undifined imm in %s\n", __func__);
            abort();
        break;
    }

    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
}

static void ECALL_impl(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
    printf("%s unimplemented\n", __func__);
}

static void EBREAK_impl(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
    printf("%s unimplemented\n", __func__);
}

void nRISC_V_cmd::FENCE_I(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
    printf("%s unimplemented\n", __func__);
}

void nRISC_V_cmd::CSRRW(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
    printf("%s unimplemented\n", __func__);
}

void nRISC_V_cmd::CSRRS(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
    printf("%s unimplemented\n", __func__);
}

 void nRISC_V_cmd::CSRRC(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
    printf("%s unimplemented\n", __func__);
}

 void nRISC_V_cmd::CSRRWI(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
    printf("%s unimplemented\n", __func__);
}

 void nRISC_V_cmd::CSRRSI(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
    printf("%s unimplemented\n", __func__);
}

 void nRISC_V_cmd::CSRRCI(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
    printf("%s unimplemented\n", __func__);
}

void nRISC_V_cmd::ADDIW(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<int32_t, 12>(static_cast<RISC_V_word_t>(instr_pkg.RV_instr_component.imm));

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];
    
    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];
    
    rd_val = se_imm + static_cast<Int_t>(rs1_val);

    // Overflows are ignored and the result is the low 32 bits of the result sign-extended to 64 bits.
    rd_val = Signed_extend<int64_t, 32>(rd_val);
}

void nRISC_V_cmd::SLLIW(Instruction_package &instr_pkg)
{
    auto shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 4, 0);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] ;

    uint32_t lower32_rs1_val = rs1_val & (RISC_V_double_word_t(0xFF'FF'FF'FF));

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    auto val = lower32_rs1_val << shamt;

    rd_val = Signed_extend<int64_t, 32>(val) ;
}

void nRISC_V_cmd::SRLIW(Instruction_package &instr_pkg)
{
    auto shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 4, 0);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] ;

    uint32_t lower32_rs1_val = rs1_val & (RISC_V_double_word_t(0xFF'FF'FF'FF));

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    auto val = lower32_rs1_val >> shamt;

    rd_val = Signed_extend<int64_t, 32>(val) ;
}

void nRISC_V_cmd::SRAIW(Instruction_package &instr_pkg)
{
    auto shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 4, 0);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] ;

    uint32_t lower32_rs1_val = rs1_val & (RISC_V_double_word_t(0xFF'FF'FF'FF));

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    auto val = static_cast<int32_t>(lower32_rs1_val) >> shamt;

    rd_val = Signed_extend<int64_t, 32>(val) ;
}

// Overflows are ignored, and the low 32-bits of the result is 
// sign-extended to 64-bits and written to the destination register.
void nRISC_V_cmd::ADDW(Instruction_package &instr_pkg)
{
    uint32_t lower32_rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & (RISC_V_double_word_t(0xFF'FF'FF'FF)); 

    uint32_t lower32_rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & (RISC_V_double_word_t(0xFF'FF'FF'FF));

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = static_cast<Int_t>(lower32_rs1_val) + static_cast<Int_t>(lower32_rs2_val);
}

void nRISC_V_cmd::SLLW(Instruction_package &instr_pkg)
{
    uint32_t lower32_rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & (RISC_V_double_word_t(0xFF'FF'FF'FF)); 

    uint32_t lower32_rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & (RISC_V_double_word_t(0xFF'FF'FF'FF));

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = Signed_extend<Int_t, 32>(lower32_rs1_val << (lower32_rs2_val & 0b11111));
}

void nRISC_V_cmd::SRLW(Instruction_package &instr_pkg)
{
    uint32_t lower32_rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & (RISC_V_double_word_t(0xFF'FF'FF'FF)); 

    uint32_t lower32_rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & (RISC_V_double_word_t(0xFF'FF'FF'FF));

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = Signed_extend<Int_t, 32>(lower32_rs1_val >> (lower32_rs2_val & 0b11111));
}

void nRISC_V_cmd::SRAW(Instruction_package &instr_pkg)
{
    uint32_t lower32_rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & (RISC_V_double_word_t(0xFF'FF'FF'FF)); 

    uint32_t lower32_rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & (RISC_V_double_word_t(0xFF'FF'FF'FF));

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = Signed_extend<Int_t, 32>(static_cast<int32_t>(lower32_rs1_val) >> (lower32_rs2_val & 0b11111));
}

// Overflows are ignored, and the low 32-bits of the result is 
// sign-extended to 64-bits and written to the destination register.
void nRISC_V_cmd::SUBW(Instruction_package &instr_pkg)
{
    uint32_t lower32_rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & (RISC_V_double_word_t(0xFF'FF'FF'FF)); 

    uint32_t lower32_rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & (RISC_V_double_word_t(0xFF'FF'FF'FF));

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = static_cast<Int_t>(lower32_rs1_val) - static_cast<Int_t>(lower32_rs2_val);
}

