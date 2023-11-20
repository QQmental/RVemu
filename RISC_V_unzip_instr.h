#pragma once

#include "RISC_V_cmd.h"

/*
compressed instruction format

Format    Meaning                   15 14 13    12 11 10 9 8 7    6 5     4 3   2 1 0
 CR        Register               |   funct4      |   rd/rs1     |      rs2      | op |
 CI        Immediate              | funct3  | imm |   rd/rs1     |      imm      | op |
 CSS       Stack-relative-Store   | funct3  |        imm         |      rs2      | op |
 CIW       Wide Immediate         | funct3  |            imm             |  rd'  | op |
 CL        Load                   | funct3  |    imm    |  rs1'  |  imm  |  rd'  | op |
 CS        Store                  | funct3  |    imm    |  rs1'  |  imm  | rs2'  | op | 
 CA        Arithmetic             |       funct6        |rd'/rs1'|funct2 | rs2'  | op |
 CB        Branch                 | funct3  |   offset  |  rs1'  |   offset      | op |
 CJ        Jump                   | funct3  |         imm(jump target)           | op |

Registers specified by the three-bit rs1’, rs2’, and rd’ fields of the CIW, CL, CS, and CB formats.

RVC Register Number                000  001  010  011  100  101  110  111
Integer Regsiter Number             x8   x9  x10  x11  x12  x13  x14  x15
Integer Regsiter ABI Name           s0   s1   a0   a1   a2   a3   a4   a5
Floting Point Regsiter Number       f8   f9  f10  f11  f12  f13  f14  f15 
Floting Point Regsiter ABI Name    fs0  fs1  fa0  fa1  fa2  fa3  fa4  fa5
*/

namespace nRISC_V_unzip_instr
{
    
class Compressed_command_map
{
public:

/*
    if the returned function has no prefix 'C_', it indicates that the non-compressed command 
    can be compatible with the compressed command
*/

    virtual nRISC_V_cmd::instr_cmd_t C_LWSP() {return nRISC_V_cmd::LW;}
    virtual nRISC_V_cmd::instr_cmd_t C_LDSP() {return nRISC_V_cmd::LD;}
    virtual nRISC_V_cmd::instr_cmd_t C_LQSP() {nUtil::FATAL("C.LQSP is not implemented\n"); return nullptr;}
    virtual nRISC_V_cmd::instr_cmd_t C_FLWSP() {nUtil::FATAL("C.FLWSP is not implemented\n"); return nullptr;}
    virtual nRISC_V_cmd::instr_cmd_t C_FLDSP() {nUtil::FATAL("C.FLDSP is not implemented\n"); return nullptr;}
    
    virtual nRISC_V_cmd::instr_cmd_t C_SWSP() {return nRISC_V_cmd::SW;}
    virtual nRISC_V_cmd::instr_cmd_t C_SDSP() {return nRISC_V_cmd::SD;}
    virtual nRISC_V_cmd::instr_cmd_t C_SQSP() {nUtil::FATAL("C.SQSP is not implemented\n"); return nullptr;}
    virtual nRISC_V_cmd::instr_cmd_t C_FSWSP() {nUtil::FATAL("C.FSWSP is not implemented\n"); return nullptr;}
    virtual nRISC_V_cmd::instr_cmd_t C_FSDSP() {nUtil::FATAL("C.FSDSP is not implemented\n"); return nullptr;}

    virtual nRISC_V_cmd::instr_cmd_t C_LW() {return nRISC_V_cmd::LW;}
    virtual nRISC_V_cmd::instr_cmd_t C_LD() {return nRISC_V_cmd::LD;}
    virtual nRISC_V_cmd::instr_cmd_t C_LQ() {nUtil::FATAL("C.LQ is not implemented\n"); return nullptr;}
    virtual nRISC_V_cmd::instr_cmd_t C_FLW() {nUtil::FATAL("C.FLW is not implemented\n"); return nullptr;}
    virtual nRISC_V_cmd::instr_cmd_t C_FLD() {nUtil::FATAL("C.FLD is not implemented\n"); return nullptr;}

    virtual nRISC_V_cmd::instr_cmd_t C_SW() {return nRISC_V_cmd::SW;}
    virtual nRISC_V_cmd::instr_cmd_t C_SD() {return nRISC_V_cmd::SD;}
    virtual nRISC_V_cmd::instr_cmd_t C_SQ() {nUtil::FATAL("C.SQ is not implemented\n"); return nullptr;}
    virtual nRISC_V_cmd::instr_cmd_t C_FSW() {nUtil::FATAL("C.FSW is not implemented\n"); return nullptr;}
    virtual nRISC_V_cmd::instr_cmd_t C_FSD() {nUtil::FATAL("C.FSD is not implemented\n"); return nullptr;}

    virtual nRISC_V_cmd::instr_cmd_t C_J() {return nRISC_V_cmd::JAl;}
    virtual nRISC_V_cmd::instr_cmd_t C_JAL() {return nRISC_V_cmd::C_JAL;}
    virtual nRISC_V_cmd::instr_cmd_t C_JR() {return nRISC_V_cmd::JALR;}
    virtual nRISC_V_cmd::instr_cmd_t C_JALR() {return nRISC_V_cmd::C_JALR;} 

    // compared with zero only
    virtual nRISC_V_cmd::instr_cmd_t C_BEQZ() {return nRISC_V_cmd::BEQ;}
    // compared with zero only
    virtual nRISC_V_cmd::instr_cmd_t C_BNEZ() {return nRISC_V_cmd::BNE;}

    virtual nRISC_V_cmd::instr_cmd_t C_LI() {return nRISC_V_cmd::ADDI;}
    virtual nRISC_V_cmd::instr_cmd_t C_LUI() 
    {
        if constexpr(nRISC_V_cmd::gXLEN == 32 || nRISC_V_cmd::gXLEN == 64)
            return nRISC_V_cmd::LUI;
        else
        {
            nUtil::FATAL("no suitable implementation for LUI\n"); 
            return nullptr;
        }
    }  

    virtual nRISC_V_cmd::instr_cmd_t C_ADDI() {return nRISC_V_cmd::ADDI;} 
    virtual nRISC_V_cmd::instr_cmd_t C_ADDIW() {return nRISC_V_cmd::ADDIW;}
    virtual nRISC_V_cmd::instr_cmd_t C_ADDI_16SP() {return nRISC_V_cmd::ADDI;}
    virtual nRISC_V_cmd::instr_cmd_t C_ADDI_4SPN() {return nRISC_V_cmd::ADDI;} 

    virtual nRISC_V_cmd::instr_cmd_t C_SLLI()
    {
        if constexpr(nRISC_V_cmd::gXLEN == 32 || nRISC_V_cmd::gXLEN == 64)
            return nRISC_V_cmd::SLLI;
        else
        {
            nUtil::FATAL("no suitable implementation for SLLI\n"); 
            return nullptr;
        }
    }

    virtual nRISC_V_cmd::instr_cmd_t C_SRLI()
    {
        if constexpr(nRISC_V_cmd::gXLEN == 32 || nRISC_V_cmd::gXLEN == 64)
            return nRISC_V_cmd::SRLI;
        else
        {
            nUtil::FATAL("no suitable implementation for SRLI\n"); 
            return nullptr;
        }
    }

    virtual nRISC_V_cmd::instr_cmd_t C_SRAI()
    {
        if constexpr(nRISC_V_cmd::gXLEN == 32 || nRISC_V_cmd::gXLEN == 64)
            return nRISC_V_cmd::SRAI;
        else
        {
            nUtil::FATAL("no suitable implementation for SRAI\n"); 
            return nullptr;
        }
    }

    virtual nRISC_V_cmd::instr_cmd_t C_ANDI() {return nRISC_V_cmd::ANDI;}     

    virtual nRISC_V_cmd::instr_cmd_t C_MV() {return nRISC_V_cmd::ADD;}     
    virtual nRISC_V_cmd::instr_cmd_t C_ADD() {return nRISC_V_cmd::ADD;}

    virtual nRISC_V_cmd::instr_cmd_t C_AND() {return nRISC_V_cmd::AND;}
    virtual nRISC_V_cmd::instr_cmd_t C_OR() {return nRISC_V_cmd::OR;}
    virtual nRISC_V_cmd::instr_cmd_t C_XOR() {return nRISC_V_cmd::XOR;}
    virtual nRISC_V_cmd::instr_cmd_t C_SUB() {return nRISC_V_cmd::SUB;}
    virtual nRISC_V_cmd::instr_cmd_t C_ADDW() 
    {
        if constexpr(nRISC_V_cmd::gXLEN != 64 && nRISC_V_cmd::gXLEN != 128)
            nUtil::FATAL("no suitable implementation for ADDW\n");
        else
            return nRISC_V_cmd::ADDW;
    }
    virtual nRISC_V_cmd::instr_cmd_t C_SUBW()
    {
        if constexpr(nRISC_V_cmd::gXLEN != 64 && nRISC_V_cmd::gXLEN != 128)
            nUtil::FATAL("no suitable implementation for SUBW\n");
        else
            return nRISC_V_cmd::SUBW;
    }    
    
    virtual nRISC_V_cmd::instr_cmd_t C_Illegal_instruction() {return nRISC_V_cmd::Illegal_CMD;}

    virtual nRISC_V_cmd::instr_cmd_t C_NOP() {return nRISC_V_cmd::C_NOP;}

    virtual nRISC_V_cmd::instr_cmd_t C_EBREAK() {return nRISC_V_cmd::EBREAK;}

    virtual nRISC_V_cmd::instr_cmd_t C_HINT() {return nRISC_V_cmd::HINT;}
};





void Unzip_compressed_instr(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, 
                            nRISC_V_cpu_spec::RV_Instr_component &component,
                            nRISC_V_cmd::instr_cmd_t *dst_cmd);
}