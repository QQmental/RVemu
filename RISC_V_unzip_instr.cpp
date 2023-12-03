#include <assert.h>

#include "RISC_V_unzip_instr.h"

using namespace nRISC_V_cpu_spec;

static RISC_V_Instr_t 
Extract_portion_of_instruction(const RISC_V_Instr_t &targ, uint32_t upper_bit_pos, uint32_t lower_bit_pos);

static auto const EPOI = Extract_portion_of_instruction;

struct Compressed_instr_component
{
    RISC_V_Instr_t opcode, rd, rs1, rs2, funct2, funct3, funct4, funct6, imm;
};

static RISC_V_Instr_t Parse_J_imm(RISC_V_Instr_t instruction);

static void Decompose_CR(RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CI(RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CSS(RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CIW(RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CL(RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CS(RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CA(RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CB(RISC_V_Instr_t instruction, Compressed_instr_component &component);
static void Decompose_CJ(RISC_V_Instr_t instruction, Compressed_instr_component &component);

// the 3 bits correspond to inst[15:13]
// after unzipping the instruction, the arguments fit need of the non-compressed cmd.
// if any argument is specified in the compressed cmd, the argument is initilized to the specific value.  
static bool unzip_000(RISC_V_Instr_t, RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*, nRISC_V_unzip_instr::Compressed_command_map&);
static bool unzip_001(RISC_V_Instr_t, RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*, nRISC_V_unzip_instr::Compressed_command_map&);
static bool unzip_010(RISC_V_Instr_t, RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*, nRISC_V_unzip_instr::Compressed_command_map&);
static bool unzip_011(RISC_V_Instr_t, RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*, nRISC_V_unzip_instr::Compressed_command_map&);
static bool unzip_100_01(RISC_V_Instr_t, RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*, nRISC_V_unzip_instr::Compressed_command_map&);
static bool unzip_100(RISC_V_Instr_t, RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*, nRISC_V_unzip_instr::Compressed_command_map&);
static bool unzip_101(RISC_V_Instr_t, RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*, nRISC_V_unzip_instr::Compressed_command_map&);
static bool unzip_110(RISC_V_Instr_t, RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*, nRISC_V_unzip_instr::Compressed_command_map&);
static bool unzip_111(RISC_V_Instr_t, RV_Instr_component&, nRISC_V_cmd::instr_cmd_t*, nRISC_V_unzip_instr::Compressed_command_map&);


template <typename ret_Int_t, unsigned B>
ret_Int_t Signed_extend(RISC_V_double_word_t num)
{
    struct {ret_Int_t x:B;} s;
    
    s.x = num;
    
    return s.x;
}

static RISC_V_Instr_t 
Extract_portion_of_instruction(const RISC_V_Instr_t &targ, uint32_t upper_bit_pos, uint32_t lower_bit_pos)
{
    assert(upper_bit_pos >= lower_bit_pos);

    auto left_shift_len = sizeof(RISC_V_Instr_t) * std::numeric_limits<uint8_t>::digits - 1 - upper_bit_pos;
    auto t = targ << left_shift_len;
    return t >> (sizeof(RISC_V_Instr_t) * std::numeric_limits<uint8_t>::digits - 1 - upper_bit_pos + lower_bit_pos);
}

static RISC_V_Instr_t Parse_J_imm(RISC_V_Instr_t instruction)
{
    RISC_V_Instr_t ret;
    auto imm_11 = EPOI(instruction, 12, 12);
    auto imm_4 = EPOI(instruction, 11, 11);
    auto imm_9_8 = EPOI(instruction, 10, 9);
    auto imm_10 = EPOI(instruction, 8, 8);
    auto imm_6 = EPOI(instruction, 7, 7);
    auto imm_7 = EPOI(instruction, 6, 6);
    auto imm_3_1 = EPOI(instruction, 5, 3);
    auto imm_5 = EPOI(instruction, 2, 2);
    ret = (imm_11 << 11) | (imm_4 << 4) | (imm_9_8 << 8) | (imm_10 << 10) | (imm_6 << 6) | \
    (imm_7 << 7) | (imm_3_1 << 1) | (imm_5 << 5);

    
    return ret;
}

static void Decompose_CR(RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct4 = EPOI(instruction, 15, 12);
    component.rd = component.rs1 = EPOI(instruction, 11, 7);
    component.rs2 = EPOI(instruction, 6, 2);
    component.opcode = EPOI(instruction, 1, 0);
}

static void Decompose_CI(RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = EPOI(instruction, 15, 13);
    component.rd = component.rs1 = EPOI(instruction, 11, 7);
    component.imm = (EPOI(instruction, 12, 12) << 5)
                  | EPOI(instruction, 6, 2);
    
    component.opcode = EPOI(instruction, 1, 0);
}

static void Decompose_CSS(RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = EPOI(instruction, 15, 13);
    component.imm = EPOI(instruction, 12, 7);
    component.rs2 = EPOI(instruction, 6, 2);
    component.opcode = EPOI(instruction, 1, 0);
}

static void Decompose_CIW(RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = EPOI(instruction, 15, 13);    
    component.imm = EPOI(instruction, 12, 5);
    component.rd = EPOI(instruction, 4, 2);
    component.opcode = EPOI(instruction, 1, 0);
}

static void Decompose_CL(RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = EPOI(instruction, 15, 13);    
    component.imm = (EPOI(instruction, 12, 10) << 2) 
                  | EPOI(instruction, 6, 5);
    
    component.rs1 = EPOI(instruction, 9, 7);    
    component.rd = EPOI(instruction, 4, 2);
    component.opcode = EPOI(instruction, 1, 0);    
}

static void Decompose_CS(RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = EPOI(instruction, 15, 13);    
    component.imm = (EPOI(instruction, 12, 10) << 2)
                  | EPOI(instruction, 6, 5);
    
    component.rs1 = EPOI(instruction, 9, 7);    
    component.rs2 = EPOI(instruction, 4, 2);
    component.opcode = EPOI(instruction, 1, 0);    
}

static void Decompose_CA(RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct6 = EPOI(instruction, 15, 10);
    component.rd = component.rs1 = EPOI(instruction, 9, 7);
    component.rs2 = EPOI(instruction, 4, 2);    
    component.opcode = EPOI(instruction, 1, 0);    
}

static void Decompose_CB(RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = EPOI(instruction, 15, 13);    
    component.rs1 = EPOI(instruction, 9, 7);
    component.imm = (EPOI(instruction, 12, 10) << 5)
                  | EPOI(instruction, 6, 2);
    
    component.opcode = EPOI(instruction, 1, 0);    
}

static void Decompose_CJ(RISC_V_Instr_t instruction, Compressed_instr_component &component)
{
    component.funct3 = EPOI(instruction, 15, 13);    
    component.imm = EPOI(instruction, 12, 2);
    component.opcode = EPOI(instruction, 1, 0);    
}



static bool unzip_000(RISC_V_Instr_t instruction, 
                      RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd,
                      nRISC_V_unzip_instr::Compressed_command_map &cmd_map)
{
    auto ret = true;

    Compressed_instr_component cmprs_component;

    switch(EPOI(instruction, 1, 0))
    {
        case 0b00:
        {
            auto i_12_5 = EPOI(instruction, 12, 5);
            auto i_4_2 = EPOI(instruction, 4, 2);
            if (i_12_5 == 0 && i_4_2 == 0)
                *cmd = cmd_map.C_Illegal_instruction();
            else
            {
                *cmd = cmd_map.C_ADDI_4SPN();
                Decompose_CIW(instruction, cmprs_component);
                component.rd = cmprs_component.rd + RV_reg_file::reg_num::x8;
                component.rs1 = RV_reg_file::reg_num::x2;
                component.imm = (EPOI(instruction, 7, 6) << 4)
                              | (EPOI(instruction, 5, 2) << 6)
                              | (EPOI(instruction, 1, 1) << 2)
                              | (EPOI(instruction, 0, 0) << 3);
            }
        }
        break;
        case 0b01:
        {
            Decompose_CI(instruction, cmprs_component);
            if (cmprs_component.imm == 0)
            {
                if (cmprs_component.rd == 0)
                {
                    *cmd = cmd_map.C_NOP();
                    component.rd = component.rs1 = 0;
                    component.imm = cmprs_component.imm;
                }
                else
                    *cmd = cmd_map.C_HINT();
            }
            else
            {
                if (cmprs_component.rd == 0)
                    *cmd = cmd_map.C_HINT();
                else
                {
                    *cmd = cmd_map.C_ADDI();
                    component.rd = component.rs1 = cmprs_component.rd;
                    component.imm = Signed_extend<Int_t, 6>(cmprs_component.imm);
                }
            }
        }
        break;
        case 0b10:
        {
            Decompose_CI(instruction, cmprs_component);
            if (cmprs_component.rd != 0)
            {
                if constexpr(nRISC_V_cmd::gXLEN == 32 || nRISC_V_cmd::gXLEN == 64 || nRISC_V_cmd::gXLEN == 128)
                {
                    *cmd = cmd_map.C_SLLI();
                    component.rd = component.rs1 = cmprs_component.rd;
                    component.imm = component.imm;
                }

                if constexpr(nRISC_V_cmd::gXLEN == 32)
                {
                    if (component.imm & 0b10000)
                        nUtil::TODO("shamt[5] = 1 is reserved for custom extension\n");
                    if (component.imm == 0)
                        *cmd = cmd_map.C_HINT();
                }
                else if (nRISC_V_cmd::gXLEN == 64)
                {
                    if (component.imm == 0)
                        *cmd = cmd_map.C_HINT();                    
                }
                else if (nRISC_V_cmd::gXLEN == 128)
                {
                    if (component.imm == 0)
                        component.imm = 64;                    
                }
            }
            else
            {
                if constexpr(nRISC_V_cmd::gXLEN == 32)
                {
                    if (component.imm & 0b10000)
                        nUtil::TODO("shamt[5] = 1 is reserved for custom extension\n");
                    else
                        *cmd = cmd_map.C_HINT();
                }
                else
                    *cmd = cmd_map.C_HINT();                 
            }
        }
        break;
        default:
            abort();
        break;
    }
    return ret;
}


static bool unzip_001(RISC_V_Instr_t instruction, 
                      RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd,
                      nRISC_V_unzip_instr::Compressed_command_map &cmd_map)
{
    auto ret = true;

    Compressed_instr_component cmprs_component;

    switch(EPOI(instruction, 1, 0))
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
                *cmd = cmd_map.C_JAL();
                component.rd = RV_reg_file::x1;
                component.imm = Parse_J_imm(instruction);

           }
           else
           {
                Decompose_CI(instruction, cmprs_component);
                if (cmprs_component.rd == 0)
                    nUtil::TODO("a reserved cmd\n");
                else
                {   
                    *cmd = cmd_map.C_ADDIW();
                    component.rd = component.rs1 = cmprs_component.rd;
                    component.imm = Signed_extend<Int_t, 6>(cmprs_component.imm);
                }
           }
        }
        break;
        case 0b10:
        {
            /*C.FLDSP (RV32/64) C.LQSP(RV128)*/
            ret = false;
            break;
        }

        default:
            abort();
        break;
    }
    return ret;
}                    

static bool unzip_010(RISC_V_Instr_t instruction, 
                      RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd,
                      nRISC_V_unzip_instr::Compressed_command_map &cmd_map)
{
    auto ret = true;

    Compressed_instr_component cmprs_component;

    switch(EPOI(instruction, 1, 0))
    {
        case 0b00:
        {
            Decompose_CL(instruction, cmprs_component);
            *cmd = cmd_map.C_LW();
            component.rd = cmprs_component.rd + RV_reg_file::x8;
            component.rs1 = cmprs_component.rs1 + RV_reg_file::x8;
            component.imm = (EPOI(cmprs_component.imm, 4, 2) << 3)
                          | (EPOI(cmprs_component.imm, 1, 1) << 2)
                          | (EPOI(cmprs_component.imm, 0, 0) << 6);
        }
        break;
        case 0b01:
        {
            Decompose_CI(instruction, cmprs_component);
            if (cmprs_component.rd == 0)
                *cmd = cmd_map.C_HINT();
            else
            {
                *cmd = cmd_map.C_LI();
                component.rd = cmprs_component.rd;
                component.rs1 = RV_reg_file::x0;
                component.imm = Signed_extend<Int_t, 6>(cmprs_component.imm);
            }
        }
        break;
        case 0b10:
        {
            Decompose_CI(instruction, cmprs_component);
            if (cmprs_component.rd == 0)
                nUtil::TODO("a reserved cmd\n");
            else
            {
                *cmd = cmd_map.C_LWSP();
                component.rd = cmprs_component.rd;
                component.rs1 = RV_reg_file::x2;
                component.imm = (EPOI(cmprs_component.imm, 5, 5) << 5)
                              | (EPOI(cmprs_component.imm, 4, 2) << 2)
                              | (EPOI(cmprs_component.imm, 1, 0) << 6);
            }      
        }
        break;
        default:
            abort();
        break;
    }

    return ret;
}       

static bool unzip_011(RISC_V_Instr_t instruction, 
                      RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd,
                      nRISC_V_unzip_instr::Compressed_command_map &cmd_map)
{
    auto ret = true;

    Compressed_instr_component cmprs_component;

    switch(EPOI(instruction, 1, 0))
    {
        case 0b00:
        {
            if constexpr(nRISC_V_cmd::gXLEN == 32)
            {
                Decompose_CI(instruction, cmprs_component);
                *cmd = cmd_map.C_FLW();
                component.rd = cmprs_component.rd + RV_reg_file::x8;
                component.rs1 = cmprs_component.rs1 + RV_reg_file::x8;
                component.imm = (EPOI(cmprs_component.imm, 4, 2) << 3)
                              | (EPOI(cmprs_component.imm, 1, 1) << 2)
                              | (EPOI(cmprs_component.imm, 0, 0) << 6);
            }
            else
            {
                Decompose_CI(instruction, cmprs_component);
                *cmd = cmd_map.C_LD();
                component.rd = cmprs_component.rd + RV_reg_file::x8;
                component.rs1 = cmprs_component.rs1 + RV_reg_file::x8;
                component.imm = (EPOI(cmprs_component.imm, 4, 2) << 3)
                              | (EPOI(cmprs_component.imm, 1, 0) << 6); 
            }
        }
        break;
        case 0b01:
        {
            Decompose_CI(instruction, cmprs_component);
            if (cmprs_component.rd == 2)
            {
                if (cmprs_component.imm == 0)
                    nUtil::TODO("reserved function\n");
                else
                {
                    *cmd = cmd_map.C_ADDI_16SP();
                    component.rd = component.rs1 = RV_reg_file::x2;
                    component.imm = (EPOI(cmprs_component.imm, 5, 5) << 9)
                                  | (EPOI(cmprs_component.imm, 4, 4) << 4)
                                  | (EPOI(cmprs_component.imm, 3, 3) << 6)
                                  | (EPOI(cmprs_component.imm, 2, 1) << 7)
                                  | (EPOI(cmprs_component.imm, 0, 0) << 5);
                    component.imm = Signed_extend<Int_t, 10>(component.imm);
                }
            }
            else
            {
                if (cmprs_component.imm == 0)
                    nUtil::TODO("reserved function\n");
                else if (cmprs_component.rd == 0)
                    *cmd = cmd_map.C_HINT();
                else
                {
                    *cmd = cmd_map.C_LUI();
                    component.rd = cmprs_component.rd;
                    component.imm = cmprs_component.imm << 12;
                    component.imm =  Signed_extend<Int_t, 18>(component.imm);
                }
            }
        }
        break;
        case 0b10:
        {
            Decompose_CI(instruction, cmprs_component);
            if constexpr(nRISC_V_cmd::gXLEN == 32) //C.FLWSP
            {
                *cmd = cmd_map.C_FLWSP();
                component.rd = cmprs_component.rd;
                component.rs1 = RV_reg_file::x2;
                component.imm = (EPOI(cmprs_component.imm, 5, 2) << 2)
                              | (EPOI(cmprs_component.imm, 1, 0) << 6); 
            }
            else //C.LDSP
            {
                if (cmprs_component.rd == 0)
                    nUtil::TODO("reserved function\n");
                else
                {
                    *cmd = cmd_map.C_LDSP();
                    component.rd = cmprs_component.rd;
                    component.rs1 = RV_reg_file::x2;
                    component.imm = (EPOI(cmprs_component.imm, 5, 3) << 3)
                                  | (EPOI(cmprs_component.imm, 2, 0) << 6); 
                }                    
            }
        }
        break;
        default:
            abort();
        break;;
    }
    return ret;
}       

static bool unzip_100_01(RISC_V_Instr_t instruction, 
                      RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd,
                      nRISC_V_unzip_instr::Compressed_command_map &cmd_map)
{
    auto ret = true;
    Compressed_instr_component cmprs_component;

    if (EPOI(instruction, 11, 10) == 0b11)
    {
        Decompose_CA(instruction, cmprs_component);
        component.rd = component.rs1 = cmprs_component.rs1 + RV_reg_file::x8;
        component.rs2 = cmprs_component.rs2 + RV_reg_file::x8;

        if (EPOI(instruction, 12, 12) == 0b0)
        {
            switch(cmprs_component.funct2)
            {
                case 0b00:
                    *cmd = cmd_map.C_SUB();
                break;
                case 0b01:
                    *cmd = cmd_map.C_XOR();
                break;
                case 0b10:
                    *cmd = cmd_map.C_OR();
                break;
                case 0b11:
                    *cmd = cmd_map.C_AND();
                break;                        
            }
        }
        else
        {
            switch(cmprs_component.funct2)
            {
                case 0b00:
                {
                    if constexpr(nRISC_V_cmd::gXLEN == 64 || nRISC_V_cmd::gXLEN == 128)
                        *cmd = cmd_map.C_SUBW();
                    else
                        nUtil::TODO("reserved function\n");    
                }
                break;
                case 0b01:
                {
                    if constexpr(nRISC_V_cmd::gXLEN == 64 || nRISC_V_cmd::gXLEN == 128)
                        *cmd = cmd_map.C_ADDW();
                    else
                        nUtil::TODO("reserved function\n");                              
                }
                break;
                case 0b10:
                    nUtil::TODO("reserved function\n"); 
                break;
                case 0b11:
                    nUtil::TODO("reserved function\n"); 
                break;
                default:
                    abort();
                break;                        
            }       
        }
    }
    else
    {
        Decompose_CB(instruction, cmprs_component);
        component.rd = component.rs1 = cmprs_component.rs1 + RV_reg_file::x8;
        component.imm = (EPOI(cmprs_component.imm, 7, 7) << 5) | EPOI(cmprs_component.imm, 4, 0);
        switch(EPOI(instruction, 11, 10))
        {
            case 0b00:
            {
                *cmd = cmd_map.C_SRLI();
                if constexpr(nRISC_V_cmd::gXLEN == 128)
                    nUtil::TODO("RV128 C.SRLI is not implemented\n");
                
                if constexpr(nRISC_V_cmd::gXLEN == 32)
                {
                    if (EPOI(instruction, 12, 12) == 0b1)
                        nUtil::TODO("reserved function\n");
                    else if (component.imm == 0)
                        *cmd = cmd_map.C_HINT();
                }
                else if(nRISC_V_cmd::gXLEN == 64)
                {
                    if (component.imm == 0)
                        *cmd = cmd_map.C_HINT();
                }                          
            }
            break;
            case 0b01:
            {
                *cmd = cmd_map.C_SRAI();
                if constexpr(nRISC_V_cmd::gXLEN == 128)
                    nUtil::TODO("RV128 C.SRAI is not implemented\n");
                if constexpr(nRISC_V_cmd::gXLEN == 32)
                {
                    if (EPOI(instruction, 12, 12) == 0b1)
                        nUtil::TODO("reserved function\n");
                    else if (component.imm == 0)
                        *cmd = cmd_map.C_HINT();
                }
                else if(nRISC_V_cmd::gXLEN == 64)
                {
                    if (component.imm == 0)
                        *cmd = cmd_map.C_HINT();
                }  
            }
            break;
            case 0b10:
            {
                *cmd = cmd_map.C_ANDI();
                component.imm = Signed_extend<Int_t, 6>(component.imm);
            }   
            break;
        }
    }
    return ret;
}                      

static bool unzip_100(RISC_V_Instr_t instruction, 
                      RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd,
                      nRISC_V_unzip_instr::Compressed_command_map &cmd_map)
{
    auto ret = true;

    switch(EPOI(instruction, 1, 0))
    {
        Compressed_instr_component cmprs_component;

        case 0b00:
            nUtil::TODO("reserved function\n");            
        break;
        case 0b01:
            unzip_100_01(instruction, component, cmd, cmd_map);
        break;
        case 0b10:
        {
            Decompose_CR(instruction, cmprs_component);
            component.rd = component.rs1 = cmprs_component.rs1;
            component.rs2 = cmprs_component.rs2;
            switch(EPOI(cmprs_component.funct4, 0, 0))
            {
                case 0b0:
                {
                    if(component.rs2 == 0)
                    {
                        if (component.rs1 == 0)
                            nUtil::TODO("reserved function\n");                            
                        else
                        {
                            *cmd = cmd_map.C_JR();
                            component.rd = RV_reg_file::x0;
                            component.imm = 0;
                        }
                    }
                    else
                    {
                        if (component.rd == 0)
                            *cmd = cmd_map.C_HINT();                            
                        else
                        {
                            component.rs1 = RV_reg_file::x0;
                            *cmd = cmd_map.C_MV();                            
                        }
                    }
                }
                break;
                case 0b1:
                {
                    if (component.rs2 != 0)
                    {
                        if (component.rd == 0)
                            *cmd = cmd_map.C_HINT();
                        else
                            *cmd = cmd_map.C_ADD();
                    }
                    else
                    {
                        if (component.rs1 == 0)
                            *cmd = cmd_map.C_EBREAK();
                        else
                        {
                            *cmd = cmd_map.C_JALR();
                            component.rd = RV_reg_file::x1;
                            component.imm = 0;
                        }
                    }
                }
                break;
            }
        }
        break;
    }
    return ret;
}       

static bool unzip_101(RISC_V_Instr_t instruction, 
                      RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd,
                      nRISC_V_unzip_instr::Compressed_command_map &cmd_map)
{
    auto ret = true;
    
    Compressed_instr_component cmprs_component;

    switch(EPOI(instruction, 1, 0))
    {
        case 0b00:
        {
            Decompose_CL(instruction, cmprs_component);
            component.rs1 = cmprs_component.rs1 + RV_reg_file::x8;
            component.rs2 = cmprs_component.rs2 + RV_reg_file::x8;
            
            if constexpr (nRISC_V_cmd::gXLEN == 32 || nRISC_V_cmd::gXLEN == 64)
            {
                component.imm = (EPOI(cmprs_component.imm, 4, 2) << 3)
                              | (EPOI(cmprs_component.imm, 1, 0) << 6);
                *cmd = cmd_map.C_FSD();
            }
            else
            {
                component.imm = (EPOI(cmprs_component.imm, 4, 3) << 4)
                              | (EPOI(cmprs_component.imm, 2, 0) << 6);
                *cmd = cmd_map.C_SQ();
            }
        }
        break;
        case 0b01:
        {
            Decompose_CJ(instruction, cmprs_component);
            *cmd = cmd_map.C_J();
            component.rd = RV_reg_file::x0;
            component.imm = Signed_extend<Int_t, 12>(Parse_J_imm(instruction));
        }
        break;
        case 0b10:
        {
            Decompose_CSS(instruction, cmprs_component);
            if constexpr(nRISC_V_cmd::gXLEN == 32 || nRISC_V_cmd::gXLEN == 64)
            {
                *cmd = cmd_map.C_FSDSP();
                component.rs1 = RV_reg_file::x2;
                component.rs2 = cmprs_component.rs2;
                component.imm = (EPOI(cmprs_component.imm, 5, 3) << 3)
                              | (EPOI(cmprs_component.imm, 2, 0) << 6);
            }
            else
            {
                *cmd = cmd_map.C_SQSP();
                component.rs1 = RV_reg_file::x2;
                component.rs2 = cmprs_component.rs2;
                component.imm = (EPOI(cmprs_component.imm, 5, 4) << 4)
                              | (EPOI(cmprs_component.imm, 3, 0) << 6);                
            }
        }
        break;
    }   
    return ret;
}

static bool unzip_110(RISC_V_Instr_t instruction, 
                      RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd,
                      nRISC_V_unzip_instr::Compressed_command_map &cmd_map)
{
    auto ret = true;
    
    Compressed_instr_component cmprs_component;

    switch(EPOI(instruction, 1, 0))
    {
        case 0b00:
        {
            Decompose_CS(instruction, cmprs_component);
            *cmd = cmd_map.C_SW();
            component.rs1 = cmprs_component.rs1 + RV_reg_file::x8;
            component.rs2 = cmprs_component.rs2 + RV_reg_file::x8;
            component.imm = (EPOI(cmprs_component.imm, 4, 1) << 2)
                          | (EPOI(cmprs_component.imm, 0, 0) << 6);        
        }
        break;
        case 0b01:
        {
            Decompose_CB(instruction, cmprs_component);
            *cmd = cmd_map.C_BEQZ();
            component.rs1 = cmprs_component.rs1 + RV_reg_file::x8;
            component.rs2 = RV_reg_file::x0;
            component.imm = (EPOI(cmprs_component.imm, 7, 7) << 8)
                          | (EPOI(cmprs_component.imm, 6, 5) << 3)
                          | (EPOI(cmprs_component.imm, 4, 3) << 6)
                          | (EPOI(cmprs_component.imm, 2, 1) << 1)
                          | (EPOI(cmprs_component.imm, 0, 0) << 5);
            component.imm = Signed_extend<Int_t, 9>(component.imm);
        }
        break;
        case 0b10:
        {
            Decompose_CSS(instruction, cmprs_component);
            *cmd = cmd_map.C_SWSP();
            component.rs1 = RV_reg_file::x2;
            component.rs2 = cmprs_component.rs2;
            component.imm = (EPOI(cmprs_component.imm, 5, 2) << 2)
                          | (EPOI(cmprs_component.imm, 1, 0) << 6);
        }
        break;
    }

    return ret;
}   

static bool unzip_111(RISC_V_Instr_t instruction, 
                      RV_Instr_component &component, 
                      nRISC_V_cmd::instr_cmd_t *cmd,
                      nRISC_V_unzip_instr::Compressed_command_map &cmd_map)
{
    auto ret = true;
    
    Compressed_instr_component cmprs_component;

    switch(EPOI(instruction, 1, 0))
    {
        case 0b00:
        {
            Decompose_CS(instruction, cmprs_component);
            component.rs1 = cmprs_component.rs1 + RV_reg_file::x8;
            component.rs2 = cmprs_component.rs2 + RV_reg_file::x8;

            if constexpr(nRISC_V_cmd::gXLEN == 32)
            {
                *cmd = cmd_map.C_FSW();

                component.imm = (EPOI(cmprs_component.imm, 4, 1) << 2)
                              | (EPOI(cmprs_component.imm, 0, 0) << 6);              
            }
            else
            {
                *cmd = cmd_map.C_SD();
                component.imm = (EPOI(cmprs_component.imm, 4, 2) << 3)
                              | (EPOI(cmprs_component.imm, 1, 0) << 6);
            }
        }
        break;
        case 0b01:
        {
            Decompose_CB(instruction, cmprs_component);
            *cmd = cmd_map.C_BNEZ();
            component.rs1 = cmprs_component.rs1 + RV_reg_file::x8;
            component.rs2 = RV_reg_file::x0;
            component.imm = (EPOI(cmprs_component.imm, 7, 7) << 8)
                          | (EPOI(cmprs_component.imm, 6, 5) << 3)
                          | (EPOI(cmprs_component.imm, 4, 3) << 6)
                          | (EPOI(cmprs_component.imm, 2, 1) << 1)
                          | (EPOI(cmprs_component.imm, 0, 0) << 5);
            component.imm = Signed_extend<Int_t, 9>(component.imm);
        }
        break;
        case 0b10:
        {
            Decompose_CSS(instruction, cmprs_component);            
            if constexpr(nRISC_V_cmd::gXLEN == 32)
            {
                *cmd = cmd_map.C_FSWSP();
                component.rs1 = RV_reg_file::x2;
                component.rs2 = cmprs_component.rs2;
                component.imm = (EPOI(cmprs_component.imm, 5, 2) << 2)
                              | (EPOI(cmprs_component.imm, 1, 0) << 6);
            }
            else
            {
                *cmd = cmd_map.C_SDSP();
                component.rs1 = RV_reg_file::x2;
                component.rs2 = cmprs_component.rs2;
                component.imm = (EPOI(cmprs_component.imm, 5, 3) << 3)
                              | (EPOI(cmprs_component.imm, 2, 0) << 6);
            }
        }
        break;
        default:
            abort();
        break;
    }

    return ret;
}   

// dst_cmd is set according to return value from 'cmd_map'
// component is set and compatible with the cmd
void nRISC_V_unzip_instr::Unzip_compressed_instr(RISC_V_Instr_t instruction, 
                                                 RV_Instr_component &component,
                                                 nRISC_V_cmd::instr_cmd_t *dst_cmd,
                                                 Compressed_command_map &cmd_map)
{
    // the least 2 bits should not be 0b11
    assert(EPOI(instruction, 1, 0) != 0b11);

    auto instr_15_13 = EPOI(instruction, 15, 13);

    auto flag = true;

    switch(instr_15_13)
    {
        case 0b000:
            flag = unzip_000(instruction, component, dst_cmd, cmd_map);
        break;

        case 0b001:
            flag = unzip_001(instruction, component, dst_cmd, cmd_map);
        break;

        case 0b010:
            flag = unzip_010(instruction, component, dst_cmd, cmd_map);
        break;

        case 0b011:
            flag = unzip_011(instruction, component, dst_cmd, cmd_map);
        break;

        case 0b100:
            flag = unzip_100(instruction, component, dst_cmd, cmd_map);
        break;

        case 0b101:
            flag = unzip_101(instruction, component, dst_cmd, cmd_map);
        break;

        case 0b110:
            flag = unzip_110(instruction, component, dst_cmd, cmd_map);
        break;

        case 0b111:
            flag = unzip_111(instruction, component, dst_cmd, cmd_map);
        break;

        default:
            abort();
        break;
    }

    assert(flag == true);
}                                                