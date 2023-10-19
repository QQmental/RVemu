#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "RISC_V_emu.h"
#include "elf.h"
#include "RISC_V_spec.h"

enum execution_exception : uint16_t
{
    none = 0,
    divided_by_zero = 1
};

struct Instruction_package
{
    RV64I_regster_file *regs;
    execution_exception except;
    RISC_V_Instr_t instruction;
    RV64_Instr_component RV64_instr_component;
};



template <unsigned B>
int64_t Signed_extend(RISC_V_double_word_t num);

static void ADDI(Instruction_package &instr_pkg);





template <unsigned B>
static int64_t Signed_extend(RISC_V_double_word_t num)
{
    struct {int64_t x:B;} s;
    
    s.x = num;
    
    return s.x;
}

static void ADDI(Instruction_package &instr_pkg)
{
    auto se_imm = Signed_extend<12>(static_cast<RISC_V_double_word_t>(instr_pkg.RV64_instr_component.imm));

    auto rs1_val = instr_pkg.regs->gp_regs[instr_pkg.RV64_instr_component.rs1];
    
    auto &rd_val = instr_pkg.regs->gp_regs[instr_pkg.RV64_instr_component.rd];
    
    rd_val = se_imm + static_cast<int64_t>(rs1_val);
}

static void ADD(Instruction_package &instr_pkg)
{
    auto rs1_val = instr_pkg.regs->gp_regs[instr_pkg.RV64_instr_component.rs1]; 

    auto rs2_val = instr_pkg.regs->gp_regs[instr_pkg.RV64_instr_component.rs2];

    auto &rd_val = instr_pkg.regs->gp_regs[instr_pkg.RV64_instr_component.rd];

    rd_val = static_cast<int64_t>(rs1_val) + static_cast<int64_t>(rs2_val);
}