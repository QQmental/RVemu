#include <assert.h>

#include "RISC_V_unzip_instr.h"

using namespace nRISC_V_cpu_spec;

struct Compressed_instr_component
{
    RISC_V_Instr_t opcode, rd, rs1, rs2, funct2, funct3, funct4, funct6, imm;
};

static void Decompose_CR(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CI(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CSS(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CIW(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CL(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CS(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CA(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CB(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CJ(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component);

inline RISC_V_Instr_t 
Extract_portion_of_instruction(const RISC_V_Instr_t &targ, uint32_t upper_bit_pos, uint32_t lower_bit_pos);

// the 3 bits correspond to inst[15:13]
static bool unzip_000(nRISC_V_cpu_spec::RISC_V_Instr_t, nRISC_V_cpu_spec::RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*);
static bool unzip_001(nRISC_V_cpu_spec::RISC_V_Instr_t, nRISC_V_cpu_spec::RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*);
static bool unzip_010(nRISC_V_cpu_spec::RISC_V_Instr_t, nRISC_V_cpu_spec::RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*);
static bool unzip_011(nRISC_V_cpu_spec::RISC_V_Instr_t, nRISC_V_cpu_spec::RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*);
static bool unzip_100(nRISC_V_cpu_spec::RISC_V_Instr_t, nRISC_V_cpu_spec::RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*);
static bool unzip_101(nRISC_V_cpu_spec::RISC_V_Instr_t, nRISC_V_cpu_spec::RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*);
static bool unzip_110(nRISC_V_cpu_spec::RISC_V_Instr_t, nRISC_V_cpu_spec::RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*);
static bool unzip_111(nRISC_V_cpu_spec::RISC_V_Instr_t, nRISC_V_cpu_spec::RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*);


template <typename ret_Int_t, unsigned B>
ret_Int_t Signed_extend(RISC_V_double_word_t num)
{
    struct {ret_Int_t x:B;} s;
    
    s.x = num;
    
    return s.x;
}

inline RISC_V_Instr_t 
Extract_portion_of_instruction(const RISC_V_Instr_t &targ, uint32_t upper_bit_pos, uint32_t lower_bit_pos)
{
    assert(upper_bit_pos >= lower_bit_pos);

    auto left_shift_len = sizeof(RISC_V_Instr_t) * std::numeric_limits<uint8_t>::digits - 1 - upper_bit_pos;
    auto t = targ << left_shift_len;
    return t >> (sizeof(RISC_V_Instr_t) * std::numeric_limits<uint8_t>::digits - 1 - upper_bit_pos + lower_bit_pos);
}

static void Decompose_CR(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct4 = Extract_portion_of_instruction(instruction, 15, 12);
    component.rd = component.rs1 = Extract_portion_of_instruction(instruction, 11, 7);
    component.rs2 = Extract_portion_of_instruction(instruction, 6, 2);
    component.opcode = Extract_portion_of_instruction(instruction, 1, 0);
}

static void Decompose_CI(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = Extract_portion_of_instruction(instruction, 15, 13);
    component.rd = component.rs1 = Extract_portion_of_instruction(instruction, 11, 7);
    component.imm = Extract_portion_of_instruction(instruction, 12, 12) 
                  | Extract_portion_of_instruction(instruction, 6, 2);
    
    component.opcode = Extract_portion_of_instruction(instruction, 1, 0);
}

static void Decompose_CSS(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = Extract_portion_of_instruction(instruction, 15, 13);
    component.imm = Extract_portion_of_instruction(instruction, 12, 7);
    component.rs2 = Extract_portion_of_instruction(instruction, 6, 2);
    component.opcode = Extract_portion_of_instruction(instruction, 1, 0);
}

static void Decompose_CIW(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = Extract_portion_of_instruction(instruction, 15, 13);    
    component.imm = Extract_portion_of_instruction(instruction, 12, 5);
    component.rd = Extract_portion_of_instruction(instruction, 4, 2);
    component.opcode = Extract_portion_of_instruction(instruction, 1, 0);
}

static void Decompose_CL(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = Extract_portion_of_instruction(instruction, 15, 13);    
    component.imm = Extract_portion_of_instruction(instruction, 12, 10) 
                  | Extract_portion_of_instruction(instruction, 6, 5);
    
    component.rs1 = Extract_portion_of_instruction(instruction, 9, 7);    
    component.rd = Extract_portion_of_instruction(instruction, 4, 2);
    component.opcode = Extract_portion_of_instruction(instruction, 1, 0);    
}

static void Decompose_CS(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = Extract_portion_of_instruction(instruction, 15, 13);    
    component.imm = Extract_portion_of_instruction(instruction, 12, 10) 
                  | Extract_portion_of_instruction(instruction, 6, 5);
    
    component.rs1 = Extract_portion_of_instruction(instruction, 9, 7);    
    component.rs2 = Extract_portion_of_instruction(instruction, 4, 2);
    component.opcode = Extract_portion_of_instruction(instruction, 1, 0);    
}

static void Decompose_CA(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct6 = Extract_portion_of_instruction(instruction, 15, 10);
    component.rd = component.rs1 = Extract_portion_of_instruction(instruction, 9, 7);
    component.rs2 = Extract_portion_of_instruction(instruction, 4, 2);    
    component.opcode = Extract_portion_of_instruction(instruction, 1, 0);    
}

static void Decompose_CB(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = Extract_portion_of_instruction(instruction, 15, 13);    
    component.rs1 = Extract_portion_of_instruction(instruction, 9, 7);
    component.imm = Extract_portion_of_instruction(instruction, 12, 10) 
                  | Extract_portion_of_instruction(instruction, 6, 2);
    
    component.opcode = Extract_portion_of_instruction(instruction, 1, 0);    
}

static void Decompose_CJ(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = Extract_portion_of_instruction(instruction, 15, 13);    
    component.imm = Extract_portion_of_instruction(instruction, 12, 2);
    component.opcode = Extract_portion_of_instruction(instruction, 1, 0);    
}



static bool unzip_000(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, 
                      nRISC_V_cpu_spec::RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd)
{
    auto ret = true;
    switch(Extract_portion_of_instruction(instruction, 1, 0))
    {
        case 0b00:
        {

        }
        break;
        case 0b01:
        {

        }
        break;
        case 0b10:
        {

        }
        break;
        default:
            abort();
        break;
    }
    return ret;
}


static bool unzip_001(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, 
                      nRISC_V_cpu_spec::RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd)
{
    auto ret = true;
    switch(Extract_portion_of_instruction(instruction, 1, 0))
    {
        case 0b00:
        {
            /*
                C.FLD (RV32/64)
                C.LQ (RV128)
                these functions are not supproted yet.
            */
           ret = false;
        }
        break;
        case 0b01:
        {
            /* C.JAL (RV32) ; C.ADDIW (RV64/128; RES, rd=0) */
           if constexpr(nRISC_V_cmd::gXLEN == 32)
           {
                component.rd = nRISC_V_cpu_spec::RV_reg_file::x0;

                auto imm_11 = Extract_portion_of_instruction(instruction, 12, 12);
                auto imm_4 = Extract_portion_of_instruction(instruction, 11, 11);
                auto imm_9_8 = Extract_portion_of_instruction(instruction, 10, 9);
                auto imm_10 = Extract_portion_of_instruction(instruction, 8, 8);
                auto imm_6 = Extract_portion_of_instruction(instruction, 7, 7);
                auto imm_7 = Extract_portion_of_instruction(instruction, 6, 6);
                auto imm_3_1 = Extract_portion_of_instruction(instruction, 5, 3);
                auto imm_5 = Extract_portion_of_instruction(instruction, 2, 2);
                component.imm = (imm_11 << 11) | (imm_4 << 4) | (imm_9_8 << 8) | (imm_10 << 10) | (imm_6 << 6) | \
                (imm_7 << 7) | (imm_3_1 << 1) | (imm_5 << 5);
                
                *cmd = nRISC_V_cmd::JAl;
           }
           else
           {

           }
        }


        default:
            abort();
        break;
    }
    return ret;
}                    

static bool unzip_010(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, 
                      nRISC_V_cpu_spec::RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd)
{
    auto ret = true;

    return ret;
}       

static bool unzip_011(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, 
                      nRISC_V_cpu_spec::RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd)
{
    auto ret = true;

    return ret;
}       

static bool unzip_100(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, 
                      nRISC_V_cpu_spec::RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd)
{
    auto ret = true;

    return ret;
}       

static bool unzip_101(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, 
                      nRISC_V_cpu_spec::RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd)
{
    auto ret = true;

    return ret;
}       

static bool unzip_110(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, 
                      nRISC_V_cpu_spec::RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd)
{
    auto ret = true;

    return ret;
}   

static bool unzip_111(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, 
                      nRISC_V_cpu_spec::RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd)
{
    auto ret = true;

    return ret;
}   

void nRISC_V_unzip_instr::Unzip_compressed_instr(nRISC_V_cpu_spec::RISC_V_Instr_t instruction, 
                                                 nRISC_V_cpu_spec::RV_Instr_component &component,
                                                 nRISC_V_cmd::instr_cmd_t *dst_cmd)
{
    // the least 2 bits should not be 0b11
    assert(Extract_portion_of_instruction(instruction, 1, 0) != 0b11);

    auto instr_15_13 = Extract_portion_of_instruction(instruction, 15, 13);

    auto flag = true;

    switch(instr_15_13)
    {
        case 0b000:
            flag = unzip_000(instruction, component, dst_cmd);
        break;

        case 0b001:
            flag = unzip_001(instruction, component, dst_cmd);
        break;

        case 0b010:
            flag = unzip_010(instruction, component, dst_cmd);
        break;

        case 0b011:
            flag = unzip_011(instruction, component, dst_cmd);
        break;

        case 0b100:
            flag = unzip_100(instruction, component, dst_cmd);
        break;

        case 0b101:
            flag = unzip_101(instruction, component, dst_cmd);
        break;

        case 0b110:
            flag = unzip_110(instruction, component, dst_cmd);
        break;

        case 0b111:
            flag = unzip_111(instruction, component, dst_cmd);
        break;

        default:
            abort();
        break;
    }

    assert(flag == true);
}                                                