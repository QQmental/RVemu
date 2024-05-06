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

using namespace nRISC_V_cpu_spec;
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



void nRISC_V_cmd::SLLI(Instruction_package &instr_pkg);
void nRISC_V_cmd::SRLI(Instruction_package &instr_pkg);
void nRISC_V_cmd::SRAI(Instruction_package &instr_pkg);
void nRISC_V_cmd::SLL(Instruction_package &instr_pkg);

void nRISC_V_cmd::SRA(Instruction_package &instr_pkg);
void nRISC_V_cmd::LUI(Instruction_package &instr_pkg);

void nRISC_V_cmd::XORI(Instruction_package &instr_pkg);
void nRISC_V_cmd::ORI(Instruction_package &instr_pkg);
void nRISC_V_cmd::ANDI(Instruction_package &instr_pkg);
void nRISC_V_cmd::XOR(Instruction_package &instr_pkg);
void nRISC_V_cmd::OR(Instruction_package &instr_pkg);
void nRISC_V_cmd::AND(Instruction_package &instr_pkg);
void nRISC_V_cmd::LB(Instruction_package &instr_pkg);
void nRISC_V_cmd::LH(Instruction_package &instr_pkg);
void nRISC_V_cmd::LW(Instruction_package &instr_pkg);
void nRISC_V_cmd::LD(Instruction_package &instr_pkg);
void nRISC_V_cmd::LBU(Instruction_package &instr_pkg);
void nRISC_V_cmd::LHU(Instruction_package &instr_pkg);
void nRISC_V_cmd::SB(Instruction_package &instr_pkg);
void nRISC_V_cmd::SH(Instruction_package &instr_pkg);
void nRISC_V_cmd::SW(Instruction_package &instr_pkg);
void nRISC_V_cmd::SD(Instruction_package &instr_pkg);
void nRISC_V_cmd::BEQ(Instruction_package &instr_pkg);
void nRISC_V_cmd::BNE(Instruction_package &instr_pkg);
void nRISC_V_cmd::BLT(Instruction_package &instr_pkg);
void nRISC_V_cmd::BGE(Instruction_package &instr_pkg);
void nRISC_V_cmd::BLTU(Instruction_package &instr_pkg);
void nRISC_V_cmd::BGEU(Instruction_package &instr_pkg);
void nRISC_V_cmd::JAL(Instruction_package &instr_pkg);
void nRISC_V_cmd::JALR(Instruction_package &instr_pkg);

void nRISC_V_cmd::Illegal_CMD(Instruction_package &instr_pkg);
void nRISC_V_cmd::NOP(Instruction_package &instr_pkg);
void nRISC_V_cmd::HINT(Instruction_package &instr_pkg);

void nRISC_V_cmd::SYSTEM(Instruction_package &instr_pkg);
void nRISC_V_cmd::ECALL(Instruction_package &instr_pkg);
void nRISC_V_cmd::EBREAK(Instruction_package &instr_pkg);

void nRISC_V_cmd::FENCE(Instruction_package &instr_pkg);

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

void nRISC_V_cmd::C_JAL(Instruction_package &instr_pkg);
void nRISC_V_cmd::C_JALR(Instruction_package &instr_pkg);

void nRISC_V_cmd::MUL(Instruction_package &instr_pkg);
void nRISC_V_cmd::MULH(Instruction_package &instr_pkg);
void nRISC_V_cmd::MULHSU(Instruction_package &instr_pkg);
void nRISC_V_cmd::MULHU(Instruction_package &instr_pkg);
void nRISC_V_cmd::MULW(Instruction_package &instr_pkg);
void nRISC_V_cmd::DIV(Instruction_package &instr_pkg);
void nRISC_V_cmd::DIVU(Instruction_package &instr_pkg);
void nRISC_V_cmd::REM(Instruction_package &instr_pkg);
void nRISC_V_cmd::REMU(Instruction_package &instr_pkg);
void nRISC_V_cmd::DIVW(Instruction_package &instr_pkg);
void nRISC_V_cmd::DIVUW(Instruction_package &instr_pkg);
void nRISC_V_cmd::REMW(Instruction_package &instr_pkg);
void nRISC_V_cmd::REMUW(Instruction_package &instr_pkg);

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

    rd_val = rs1_val + rs2_val;
}

void nRISC_V_cmd::SUB(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val - rs2_val;
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

void nRISC_V_cmd::SLLI(Instruction_package &instr_pkg)
{
    nRISC_V_cpu_spec::RISC_V_Instr_t shamt{} ;
    
    if constexpr (nRISC_V_cmd::gXLEN == 32)
        shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 4, 0);
    else if (nRISC_V_cmd::gXLEN == 64)
        shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 5, 0);     

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val << shamt;
}

void nRISC_V_cmd::SRLI(Instruction_package &instr_pkg)
{
    nRISC_V_cpu_spec::RISC_V_Instr_t shamt{};
    
    if constexpr (nRISC_V_cmd::gXLEN == 32)
        shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 4, 0);
    else if (nRISC_V_cmd::gXLEN == 64)
        shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 5, 0);        
    
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val >> shamt;
}

// shift right arithmetic by immediate
// the original sign bit is copied into the vacated upper bits
void nRISC_V_cmd::SRAI(Instruction_package &instr_pkg)
{
    nRISC_V_cpu_spec::RISC_V_Instr_t shamt {};

    if constexpr (nRISC_V_cmd::gXLEN == 32)
        shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 4, 0);
    else if (nRISC_V_cmd::gXLEN == 64)
        shamt = nRISC_V_decompose::Extract_portion_of_instruction(instr_pkg.RV_instr_component.imm, 5, 0);
    
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    RV_int_reg_t original_sign_bit{};

    if (static_cast<Int_t>(rs1_val) < 0)
        original_sign_bit = nRISC_V_cpu_spec::RV_int_reg_t(-1);

    rd_val = (rs1_val >> shamt) | (original_sign_bit << (nRISC_V_cmd::gXLEN - shamt - 1));
}

void nRISC_V_cmd::SLL(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if constexpr (nRISC_V_cmd::gXLEN == 32)
        rd_val = rs1_val << (rs2_val & 0b11111);
    else if (nRISC_V_cmd::gXLEN == 64)
        rd_val = rs1_val << (rs2_val & RISC_V_double_word_t(0b111111));
}

void nRISC_V_cmd::SRL(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if constexpr (nRISC_V_cmd::gXLEN == 32)
        rd_val = rs1_val >> (rs2_val & 0b11111);
    else if (nRISC_V_cmd::gXLEN == 64)
        rd_val = rs1_val >> (rs2_val & RISC_V_double_word_t(0b111111));
}
// the original sign bit is copied into the vacated upper bits
void nRISC_V_cmd::SRA(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    nRISC_V_cpu_spec::RV_int_reg_t shamt{}, original_sign_bit{0};

    if constexpr (nRISC_V_cmd::gXLEN == 32)
        shamt = rs2_val & nRISC_V_cpu_spec::RV_int_reg_t(0b11111);
    else if (nRISC_V_cmd::gXLEN == 64)
        shamt = rs2_val & nRISC_V_cpu_spec::RV_int_reg_t(0b111111);
    else
        nUtil::TODO("SRA for gXLEN > 64 is not implemented\n");
    
    if (static_cast<Int_t>(rs1_val) < 0)
        original_sign_bit = nRISC_V_cpu_spec::RV_int_reg_t(-1);
    
    rd_val = (rs1_val >> shamt) | (original_sign_bit << (nRISC_V_cmd::gXLEN - shamt - 1));
}


void nRISC_V_cmd::LUI(Instruction_package &instr_pkg)
{
    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = instr_pkg.RV_instr_component.imm;
    
    CHECK_ERROR(nRISC_V_cmd::gXLEN == 64);
   
    rd_val = Signed_extend<Int_t, 32>(rd_val);
}

void nRISC_V_cmd::AUIPC(Instruction_package &instr_pkg)
{
    auto imm = Signed_extend<Int_t, 32>(instr_pkg.RV_instr_component.imm);

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

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    instr_pkg.bus.SB(rs2_val, static_cast<Int_t>(rs1_val) + se_imm);
}

void nRISC_V_cmd::SH(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 
    
    instr_pkg.bus.SH(rs2_val, static_cast<Int_t>(rs1_val) + se_imm);
}

void nRISC_V_cmd::SW(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    instr_pkg.bus.SW(rs2_val, static_cast<Int_t>(rs1_val) + se_imm);
}

void nRISC_V_cmd::SD(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    instr_pkg.bus.SD(rs2_val, static_cast<Int_t>(rs1_val) + se_imm);
}

void nRISC_V_cmd::BEQ(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 13>(instr_pkg.RV_instr_component.imm);

    if (rs1_val == rs2_val)
        instr_pkg.next_pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::BNE(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 13>(instr_pkg.RV_instr_component.imm);

    if (rs1_val != rs2_val)
        instr_pkg.next_pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::BLT(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 13>(instr_pkg.RV_instr_component.imm);

    if (static_cast<Int_t>(rs1_val) < static_cast<Int_t>(rs2_val))
        instr_pkg.next_pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::BGE(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 13>(instr_pkg.RV_instr_component.imm);

    if (static_cast<Int_t>(rs1_val) >= static_cast<Int_t>(rs2_val))
        instr_pkg.next_pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::BLTU(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 13>(instr_pkg.RV_instr_component.imm);

    if (rs1_val < rs2_val)
        instr_pkg.next_pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::BGEU(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto se_imm = Signed_extend<Int_t, 13>(instr_pkg.RV_instr_component.imm);

    if (rs1_val >= rs2_val)
        instr_pkg.next_pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::JAL(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 21>(instr_pkg.RV_instr_component.imm);
    
    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = instr_pkg.regs.pc + 4;

    instr_pkg.next_pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::JALR(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = instr_pkg.regs.pc + 4;

    instr_pkg.next_pc = (rs1_val + se_imm) & (~0x1);
}

void nRISC_V_cmd::SYSTEM(Instruction_package &instr_pkg)
{
    assert(instr_pkg.RV_instr_component.rd == 0 && instr_pkg.RV_instr_component.rs1 == 0);

    switch (instr_pkg.RV_instr_component.imm)
    {
        case 0:
            nRISC_V_cmd::ECALL(instr_pkg);
            break;
        case 1:
            nRISC_V_cmd::EBREAK(instr_pkg);
        break;            
        default:
            printf("undifined imm in %s\n", __func__);
            
        break;
    }

    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
}

void nRISC_V_cmd::ECALL(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::ecall;
    //std::cout << __func__ << " "<< std::dec << instr_pkg.regs.gp_regs[nRISC_V_cpu_spec::gp_reg_abi_name::a7] << "\n"; 
}

void nRISC_V_cmd::EBREAK(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
    printf("%s unimplemented\n", __func__);
}

void nRISC_V_cmd::FENCE(Instruction_package &instr_pkg)
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
    auto se_imm = Signed_extend<int64_t, 12>(static_cast<RISC_V_word_t>(instr_pkg.RV_instr_component.imm));

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    int64_t lower32_rs1_val = Signed_extend<int64_t, 32>(rs1_val & (RISC_V_double_word_t(0xFF'FF'FF'FF)));
    
    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];
    
    rd_val = se_imm + lower32_rs1_val;

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

    uint32_t original_sign_bit{};

    if (Signed_extend<int64_t, 32>(lower32_rs1_val) < 0)
        original_sign_bit = -1;

    auto val = (lower32_rs1_val >> shamt) | (original_sign_bit << (32 - shamt - 1)); 

    rd_val = Signed_extend<int64_t, 32>(val) ;
}

// Overflows are ignored, and the low 32-bits of the result is 
// sign-extended to 64-bits and written to the destination register.
void nRISC_V_cmd::ADDW(Instruction_package &instr_pkg)
{
    uint32_t lower32_rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & ((0xFF'FF'FF'FF)); 

    uint32_t lower32_rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & ((0xFF'FF'FF'FF));

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = Signed_extend<int64_t, 32>(lower32_rs1_val) + Signed_extend<int64_t, 32>(lower32_rs2_val);
}

void nRISC_V_cmd::SLLW(Instruction_package &instr_pkg)
{
    uint32_t lower32_rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & ((0xFF'FF'FF'FF)); 

    uint32_t lower32_rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & ((0xFF'FF'FF'FF));

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = Signed_extend<Int_t, 32>(lower32_rs1_val << (lower32_rs2_val & 0b11111));
}

void nRISC_V_cmd::SRLW(Instruction_package &instr_pkg)
{
    uint32_t lower32_rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & (0xFF'FF'FF'FF); 

    uint32_t lower32_rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & (0xFF'FF'FF'FF);

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = Signed_extend<Int_t, 32>(lower32_rs1_val >> (lower32_rs2_val & 0b11111));
}

void nRISC_V_cmd::SRAW(Instruction_package &instr_pkg)
{
    uint32_t lower32_rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & (0xFF'FF'FF'FF); 

    uint32_t lower32_rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & (0xFF'FF'FF'FF);

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = Signed_extend<Int_t, 32>(static_cast<int32_t>(lower32_rs1_val) >> (lower32_rs2_val & 0b11111));
}

// Overflows are ignored, and the low 32-bits of the result is 
// sign-extended to 64-bits and written to the destination register.
void nRISC_V_cmd::SUBW(Instruction_package &instr_pkg)
{
    uint32_t lower32_rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & ((0xFF'FF'FF'FF)); 

    uint32_t lower32_rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & ((0xFF'FF'FF'FF));

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = Signed_extend<int64_t, 32>(lower32_rs1_val) - Signed_extend<int64_t, 32>(lower32_rs2_val);
}

void nRISC_V_cmd::Illegal_CMD(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::illegal_cmd;
}

void nRISC_V_cmd::NOP(Instruction_package &instr_pkg)
{
    
}

// I am not sure about purpose of 'HINT'
void nRISC_V_cmd::HINT(Instruction_package &instr_pkg)
{
    instr_pkg.except = nRISC_V_cmd::execution_exception::finish;
    printf("%s unimplemented\n", __func__);
}

void nRISC_V_cmd::C_JAL(Instruction_package &instr_pkg)
{
    CHECK_ERROR(instr_pkg.RV_instr_component.rd == 1);

    auto se_imm = Signed_extend<Int_t, 12>(instr_pkg.RV_instr_component.imm);
    
    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = instr_pkg.regs.pc + 2;

    instr_pkg.next_pc = instr_pkg.regs.pc + se_imm;
}

void nRISC_V_cmd::C_JALR(Instruction_package &instr_pkg)
{
    CHECK_ERROR(instr_pkg.RV_instr_component.rs1 != 0);

    CHECK_ERROR(instr_pkg.RV_instr_component.imm == 0);

    CHECK_ERROR(instr_pkg.RV_instr_component.rd == 1);

    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = instr_pkg.regs.pc + 2;

    // imm should be equal to 0, so it's written in 'rs1_val + 0'
    instr_pkg.next_pc = (rs1_val + 0) & (~0x1);
}

void nRISC_V_cmd::MUL(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = rs1_val * rs2_val;
}

void nRISC_V_cmd::MULH(Instruction_package &instr_pkg)
{
    nRISC_V_cmd::gDOUBLE_XLEN_T rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    nRISC_V_cmd::gDOUBLE_XLEN_T rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto negate = (((nRISC_V_cmd::gSDOUBLE_XLEN_T )rs1_val) < 0) != (((nRISC_V_cmd::gSDOUBLE_XLEN_T )rs2_val) < 0);

    if (((nRISC_V_cmd::gSDOUBLE_XLEN_T )rs1_val) < 0)
        rs1_val = -rs1_val;
    if (((nRISC_V_cmd::gSDOUBLE_XLEN_T )rs2_val) < 0)
        rs2_val = -rs2_val;

    auto product = negate ? rs1_val * rs2_val * -1 : rs1_val * rs2_val;

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = product >> nRISC_V_cmd::gXLEN; 
}

void nRISC_V_cmd::MULHSU(Instruction_package &instr_pkg)
{
    nRISC_V_cmd::gDOUBLE_XLEN_T rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    nRISC_V_cmd::gDOUBLE_XLEN_T rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto negate = ((nRISC_V_cmd::gSDOUBLE_XLEN_T )rs1_val) < 0;

    if (((nRISC_V_cmd::gSDOUBLE_XLEN_T )rs1_val) < 0)
        rs1_val =  -rs1_val;

    auto product = negate ? rs1_val * rs2_val * -1 : rs1_val * rs2_val;

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = product >> nRISC_V_cmd::gXLEN; 
}

void nRISC_V_cmd::MULHU(Instruction_package &instr_pkg)
{
    nRISC_V_cmd::gDOUBLE_XLEN_T rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    nRISC_V_cmd::gDOUBLE_XLEN_T rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2]; 

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = (rs1_val*rs2_val) >> (nRISC_V_cmd::gXLEN); 
}

void nRISC_V_cmd::MULW(Instruction_package &instr_pkg)
{
    if constexpr(nRISC_V_cmd::gXLEN != 64)
        nUtil::FATAL("MULW is for rv64");
    
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & 0xFFFF'FFFF;

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & 0xFFFF'FFFF;

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    rd_val = Signed_extend<Int_t, 32>(rs1_val*rs2_val); 
}


void nRISC_V_cmd::DIV(Instruction_package &instr_pkg)
{
    Int_t rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    Int_t rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if (rs2_val == 0)
        rd_val = -1;
    else if (rs1_val == std::numeric_limits<Int_t>::min() && rs2_val == -1)
        rd_val = std::numeric_limits<Int_t>::min();
    else
        rd_val = rs1_val/rs2_val;
}
void nRISC_V_cmd::DIVU(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if (rs2_val == 0)
        rd_val = std::numeric_limits<Uint_t>::max();
    else
        rd_val = rs1_val/rs2_val;
         
}

void nRISC_V_cmd::REM(Instruction_package &instr_pkg)
{
    Int_t rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    Int_t rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if (rs2_val == 0)
        rd_val = rs1_val;
    else if(rs2_val == -1)
        rd_val = 0;
    else
        rd_val = rs1_val%rs2_val;
}
void nRISC_V_cmd::REMU(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1];

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2];

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if (rs2_val == 0)
        rd_val = rs1_val;
    else
        rd_val = rs1_val%rs2_val;
         
}
void nRISC_V_cmd::DIVW(Instruction_package &instr_pkg)
{
    Int_t rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & 0xFFFF'FFFF;

    Int_t rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & 0xFFFF'FFFF;

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if (rs2_val == 0)
        rd_val = -1;
    else if (rs1_val == std::numeric_limits<int32_t>::min() && rs2_val == -1)
        rd_val = std::numeric_limits<int32_t>::min();
    else
        rd_val = Signed_extend<Int_t, 32>(rs1_val/rs2_val);

         
}
void nRISC_V_cmd::DIVUW(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & 0xFFFF'FFFF;

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & 0xFFFF'FFFF;

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if (rs2_val == 0)
        rd_val = std::numeric_limits<Uint_t>::max();
    else
        rd_val = Signed_extend<Int_t, 32>(rs1_val/rs2_val);
         
}
void nRISC_V_cmd::REMW(Instruction_package &instr_pkg)
{
    Int_t rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & 0xFFFF'FFFF;

    Int_t rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & 0xFFFF'FFFF;

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if (rs2_val == 0)
        rd_val = rs1_val;
    else if(rs2_val == -1)
        rd_val = 0;
    else
        rd_val = rs1_val%rs2_val;

    rd_val = Signed_extend<Int_t, 32>(rd_val);
     
}
void nRISC_V_cmd::REMUW(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs1] & 0xFFFF'FFFF;

    auto rs2_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rs2] & 0xFFFF'FFFF;

    auto &rd_val = instr_pkg.regs.gp_regs[instr_pkg.RV_instr_component.rd];

    if (rs2_val == 0)
        rd_val = rs1_val;
    else
        rd_val = rs1_val%rs2_val;
        
    rd_val = Signed_extend<Int_t, 32>(rd_val);
}