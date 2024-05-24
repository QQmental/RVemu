#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unordered_map>

#include "RISC_V_load_guest.h"
#include "RISC_V_emu.h"
#include "RISC_V_cpu_spec.h"
#include "BUS.h"
#include "RISC_V_cmd.h"
#include "RISC_V_decompose.h"
#include "RISC_V_unzip_instr.h"
#include "Syscall.h"

using nRISC_V_cpu_spec::RISC_V_Instr_t;
using nRISC_V_cpu_spec::RV_Instr_component;

//constexpr nRISC_V_cpu_spec::RISC_V_double_word_t gDEFAULT_START_STACK_POINTER = 0x0000'0000'0400'0000;
constexpr nRISC_V_cpu_spec::RISC_V_double_word_t gRUNTIME_DATA_SPACE = 0x0000'0000'0400'0000;

constexpr nRISC_V_cpu_spec::RISC_V_double_word_t gRESERVED_SPACE = (1<<12);

enum eCPU_state : uint16_t
{
    stop = 0,
    ready = 1,
    running
};



class RISC_V_Instruction_map
{
public:
    RISC_V_Instruction_map() = default;
    ~RISC_V_Instruction_map() = default;
    struct Command_attribute
    {
        nRISC_V_cmd::instr_cmd_t cmd;
        nRISC_V_decompose::eOpecode_type op_type;
        std::string name;
    };
    
    // return true if the cmd hasn't been inserted before Regist_cmd is called.
    bool Regist_cmd(RISC_V_Instr_t cmd_mask, const RISC_V_Instruction_map::Command_attribute &attr);
    
    // a map for non-compressed instructions
    std::unordered_map<RISC_V_Instr_t, nRISC_V_cmd::instr_cmd_t> map;

};

inline bool RISC_V_Instruction_map::Regist_cmd(RISC_V_Instr_t cmd_mask, const RISC_V_Instruction_map::Command_attribute &attr)
{
    auto f = map.emplace(cmd_mask, attr.cmd);

    return f.second == true;
}

static bool Processing_instruction(const RISC_V_Instr_t &instruction, RISC_V_Instr_t *mask_dst, RV_Instr_component &component);

static void Regist_RVI_cmd(RISC_V_Instruction_map &);
static void Init_main_args(nRISC_V_cpu_spec::RV_reg_file &reg_file, BUS &bus, int argc, const char* argv[], nRISC_V_cpu_spec::RISC_V_Addr_t &brk_addr);




static void Regist_RVI_cmd(RISC_V_Instruction_map &map)
{
    map.map.reserve(1024);
    map.map.max_load_factor(0.25);
    RISC_V_Instruction_map::Command_attribute attr;

    bool regist_success {};

    #define REGIST_CMD(CMD_MASK, CMD, OP_TYPE)\
    do\
    {\
        attr.cmd = nRISC_V_cmd::CMD;\
        attr.name = #CMD;\
        attr.op_type = nRISC_V_decompose::eOpecode_type::OP_TYPE;\
        regist_success = map.Regist_cmd(CMD_MASK, attr);\
        CHECK_ERROR(regist_success == true);\
    }while(0);

    /*
        some I type functions are duplicated to reduce branches.
        I format is not consistent, leading to number of branch increases.
    */

    REGIST_CMD(0b00000000000000000000'00000'0110111, LUI,   U);
    REGIST_CMD(0b00000000000000000000'00000'0010111, AUIPC, U);

    REGIST_CMD(0b00000000000000000000'00000'1101111, JAL, J);
    
    REGIST_CMD(0b000000000000'00000'000'00000'1100111, JALR,  I);
    REGIST_CMD(0b000000000000'00000'000'00000'0000011, LB,    I);
    REGIST_CMD(0b000000000000'00000'001'00000'0000011, LH,    I);
    REGIST_CMD(0b000000000000'00000'010'00000'0000011, LW,    I);
    REGIST_CMD(0b000000000000'00000'100'00000'0000011, LBU,   I);
    REGIST_CMD(0b000000000000'00000'101'00000'0000011, LHU,   I);

    REGIST_CMD(0b000000000000'00000'000'00000'0010011, ADDI,  I); 
    REGIST_CMD(0b000000000000'00000'010'00000'0010011, SLTI,  I);
    REGIST_CMD(0b000000000000'00000'011'00000'0010011, SLTIU, I);
    REGIST_CMD(0b000000000000'00000'100'00000'0010011, XORI,  I);
    REGIST_CMD(0b000000000000'00000'110'00000'0010011, ORI,   I);
    REGIST_CMD(0b000000000000'00000'111'00000'0010011, ANDI,  I);

    REGIST_CMD(0b010000000000'00000'000'00000'0010011, ADDI,  I); 
    REGIST_CMD(0b010000000000'00000'010'00000'0010011, SLTI,  I);
    REGIST_CMD(0b010000000000'00000'011'00000'0010011, SLTIU, I);
    REGIST_CMD(0b010000000000'00000'100'00000'0010011, XORI,  I);
    REGIST_CMD(0b010000000000'00000'110'00000'0010011, ORI,   I);
    REGIST_CMD(0b010000000000'00000'111'00000'0010011, ANDI,  I);

    //RV64 SLLI, SRLI, SRAI share same format with those in RV32
    REGIST_CMD(0b000000000000'00000'001'00000'0010011, SLLI,  I);
    REGIST_CMD(0b000000000000'00000'101'00000'0010011, SRLI,  I);
    REGIST_CMD(0b010000000000'00000'101'00000'0010011, SRAI,  I);

    REGIST_CMD(0b000000000000'00000'000'00000'0001111, FENCE,  I);
    REGIST_CMD(0b000000000000'00000'000'00000'1110011, ECALL,  I);
    REGIST_CMD(0b000000000001'00000'000'00000'1110011, EBREAK,  I);

    //RV64I
    REGIST_CMD(0b000000000000'00000'110'00000'0000011, LWU,   I);
    REGIST_CMD(0b000000000000'00000'011'00000'0000011, LD,    I);

    REGIST_CMD(0b000000000000'00000'000'00000'0011011, ADDIW, I);
    REGIST_CMD(0b010000000000'00000'000'00000'0011011, ADDIW, I); 
    
    REGIST_CMD(0b000000000000'00000'001'00000'0011011, SLLIW, I);
    REGIST_CMD(0b000000000000'00000'101'00000'0011011, SRLIW, I);
    REGIST_CMD(0b010000000000'00000'101'00000'0011011, SRAIW, I);


    REGIST_CMD(0b0000000'00000'00000'000'00000'0110011, ADD,  R);    
    REGIST_CMD(0b0100000'00000'00000'000'00000'0110011, SUB,  R); 
    REGIST_CMD(0b0000000'00000'00000'001'00000'0110011, SLL,  R);
    REGIST_CMD(0b0000000'00000'00000'010'00000'0110011, SLT,  R);
    REGIST_CMD(0b0000000'00000'00000'011'00000'0110011, SLTU, R);
    REGIST_CMD(0b0000000'00000'00000'100'00000'0110011, XOR,  R);
    REGIST_CMD(0b0000000'00000'00000'101'00000'0110011, SRL,  R);
    REGIST_CMD(0b0100000'00000'00000'101'00000'0110011, SRA,  R);
    REGIST_CMD(0b0000000'00000'00000'110'00000'0110011, OR,   R);
    REGIST_CMD(0b0000000'00000'00000'111'00000'0110011, AND,  R);

    //RV32M
    REGIST_CMD(0b0000001'00000'00000'000'00000'0110011, MUL,    R);
    REGIST_CMD(0b0000001'00000'00000'001'00000'0110011, MULH,   R);
    REGIST_CMD(0b0000001'00000'00000'010'00000'0110011, MULHSU, R);
    REGIST_CMD(0b0000001'00000'00000'011'00000'0110011, MULHU,  R);
    REGIST_CMD(0b0000001'00000'00000'100'00000'0110011, DIV,    R);
    REGIST_CMD(0b0000001'00000'00000'101'00000'0110011, DIVU,   R);
    REGIST_CMD(0b0000001'00000'00000'110'00000'0110011, REM,    R);  
    REGIST_CMD(0b0000001'00000'00000'111'00000'0110011, REMU,   R);  

    //RV64M
    REGIST_CMD(0b0000001'00000'00000'000'00000'0111011, MULW,   R);
    REGIST_CMD(0b0000001'00000'00000'100'00000'0111011, DIVW,   R);
    REGIST_CMD(0b0000001'00000'00000'101'00000'0111011, DIVUW,  R);
    REGIST_CMD(0b0000001'00000'00000'110'00000'0111011, REMW,   R);
    REGIST_CMD(0b0000001'00000'00000'111'00000'0111011, REMUW,  R);

    //RV64I
    REGIST_CMD(0b0000000'00000'00000'000'00000'0111011, ADDW,  R);    
    REGIST_CMD(0b0100000'00000'00000'000'00000'0111011, SUBW,  R); 
    REGIST_CMD(0b0000000'00000'00000'001'00000'0111011, SLLW,  R);
    REGIST_CMD(0b0000000'00000'00000'101'00000'0111011, SRLW,  R);
    REGIST_CMD(0b0100000'00000'00000'101'00000'0111011, SRAW,  R);


    REGIST_CMD(0b0000000'00000'00000'000'00000'1100011, BEQ,  B);
    REGIST_CMD(0b0000000'00000'00000'001'00000'1100011, BNE,  B);
    REGIST_CMD(0b0000000'00000'00000'100'00000'1100011, BLT,  B);
    REGIST_CMD(0b0000000'00000'00000'101'00000'1100011, BGE,  B);
    REGIST_CMD(0b0000000'00000'00000'110'00000'1100011, BLTU, B);
    REGIST_CMD(0b0000000'00000'00000'111'00000'1100011, BGEU, B);
    REGIST_CMD(0b0000000'00000'00000'000'00000'0100011, SB,   S);
    REGIST_CMD(0b0000000'00000'00000'001'00000'0100011, SH,   S);
    REGIST_CMD(0b0000000'00000'00000'010'00000'0100011, SW,   S);    

    //RV64I
    REGIST_CMD(0b0000000'00000'00000'011'00000'0100011, SD,   S);

    #undef REGIST_CMD
}

// main function args are initialize when initializing stack space
// brk_addr is modified when argc and argvs are copied into memory
static void Init_main_args(nRISC_V_cpu_spec::RV_reg_file &reg_file, BUS &bus, int argc, const char* argv[], nRISC_V_cpu_spec::RISC_V_Addr_t &brk_addr)
{
    // for main function argument
    reg_file.gp_regs[nRISC_V_cpu_spec::RV_reg_file::x2] -= 8; // auxp
    reg_file.gp_regs[nRISC_V_cpu_spec::RV_reg_file::x2] -= 8; // envp
    

    for(int i = 0 ; i < argc ; i++)
    {
        strcpy(reinterpret_cast<char*>(bus.Get_raw_ptr(brk_addr)), argv[argc - i - 1]);         // copy a string from argv[]
        reg_file.gp_regs[nRISC_V_cpu_spec::RV_reg_file::x2] -= 8;                               // reserve space for a argv pointer
        void *ptr_stack = bus.Get_raw_ptr(reg_file.gp_regs[nRISC_V_cpu_spec::RV_reg_file::x2]); // pointer to new stack top location
        *reinterpret_cast<uint64_t*>(ptr_stack) = brk_addr;                                     // record argv pointer
        brk_addr += strlen(argv[argc - i - 1]) + 1;                                             // update brk_addr because of the copied string
    }

    reg_file.gp_regs[nRISC_V_cpu_spec::RV_reg_file::x2] -= 8; // argc
    *reinterpret_cast<uint64_t*>(bus.Get_raw_ptr(reg_file.gp_regs[nRISC_V_cpu_spec::RV_reg_file::x2])) = argc;    
}

RISC_V_Emulator::RISC_V_Emulator(const std::string &program_name, int argc, const char **argv)
{
    nRISC_V_load_guest::Loaded_guest_configure config(program_name, &m_program_mdata, &m_CPU_attribute, gRUNTIME_DATA_SPACE, gRESERVED_SPACE, &m_mem);

    nRISC_V_load_guest::Load_guest_program(config);

    m_instruction_map = std::make_unique<RISC_V_Instruction_map>();

    Regist_RVI_cmd(*m_instruction_map.get());
    
    m_program_mdata.CPU_attributes = &m_CPU_attribute;

    m_argc = argc;
    m_argv = argv;
}

RISC_V_Emulator::~RISC_V_Emulator()
{
    
}

// decompose instruction into components and mask instruction for map searching
// return false if the instruction has an invalid opcode
static bool Processing_instruction(const RISC_V_Instr_t &instruction, RISC_V_Instr_t *mask_dst, RV_Instr_component &component)
{
    bool opcode_type_status = true;

    switch(nRISC_V_decompose::opcode(instruction))
    {
        case 0b0110011:
        case 0b0111011:
            nRISC_V_decompose::Decompose_Rtype_instruction(component, instruction);
            *mask_dst = instruction & RISC_V_Instr_t(0b1111111'00000'00000'111'00000'1111111);
        break;

        case 0b0000011:
        case 0b1100111:
        case 0b1110011:
        case 0b0001111:
            nRISC_V_decompose::Decompose_Itype_instruction(component, instruction);
            *mask_dst = instruction & RISC_V_Instr_t(0b000000000000'00000'111'00000'1111111);
        break;

        case 0b0010011:
        case 0b0011011:
            nRISC_V_decompose::Decompose_Itype_instruction(component, instruction);
            *mask_dst = instruction & RISC_V_Instr_t(0b0100000'00000'00000'111'00000'1111111);           
        break;

        case 0b0100011:
             nRISC_V_decompose::Decompose_Stype_instruction(component, instruction);
             *mask_dst = instruction & RISC_V_Instr_t(0b0000000'00000'00000'111'00000'1111111);
        break;

        case 0b1100011:
             nRISC_V_decompose::Decompose_Btype_instruction(component, instruction);
             *mask_dst = instruction & RISC_V_Instr_t(0b0000000'00000'00000'111'00000'1111111);             
        break;

        case 0b0110111:
        case 0b0010111:
             nRISC_V_decompose::Decompose_Utype_instruction(component, instruction);
             *mask_dst = instruction & RISC_V_Instr_t(0b00000000000000000000'00000'1111111);                
        break;

        case 0b1101111:
             nRISC_V_decompose::Decompose_Jtype_instruction(component, instruction);
             *mask_dst = instruction & RISC_V_Instr_t(0b00000000000000000000'00000'1111111); 
        break;

        default:
            opcode_type_status = false;
        break;
    }

    return opcode_type_status;
}

void RISC_V_Emulator::start()
{
    BUS bus(std::move(m_mem), m_program_mdata);
    
    nRISC_V_cpu_spec::RV_reg_file reg_file = {};
    reg_file.pc = m_program_mdata.entry_point;
    reg_file.gp_regs[nRISC_V_cpu_spec::RV_reg_file::x2] = m_program_mdata.highest_addr;
    m_program_mdata.stack_top = &reg_file.gp_regs[nRISC_V_cpu_spec::gp_reg_abi_name::sp]; 

    Init_main_args(reg_file, bus, m_argc, m_argv, m_program_mdata.brk_addr);

    nRISC_V_cpu_spec::RV_Instr_component RV_instr_component = {};

    nRISC_V_cpu_spec::RISC_V_Instr_t instruction, masked_instruction;
    
    nRISC_V_cmd::Exec_component exec_component(reg_file, bus, RV_instr_component);

    nRISC_V_unzip_instr::Compressed_command_map cmprs_cmd_map;
    
    nRISC_V_cmd::instr_cmd_t cmd;

    while(1)
    {    
        bus.Fetch_instruction(reg_file, &instruction);
        
        nRISC_V_cpu_spec::RISC_V_Addr_t next_pc{};

        /* the lowest 2 bits of a compressed instruction is not 0b11*/
        if ((nRISC_V_decompose::opcode(instruction) & 0b11) != 0b11)
        {
            nRISC_V_unzip_instr::Unzip_compressed_instr(instruction, RV_instr_component, &cmd, cmprs_cmd_map);
            next_pc = reg_file.pc + 2;
        }
        else
        {
            bool opcode_type_status = Processing_instruction(instruction, &masked_instruction, RV_instr_component);

            CHECK_ERROR(opcode_type_status == true);

            auto it_instr_cmd = m_instruction_map->map.find(masked_instruction);

            CHECK_ERROR(it_instr_cmd != m_instruction_map->map.end());

            cmd = it_instr_cmd->second;
            
            next_pc = reg_file.pc + 4;
        }

        nRISC_V_cmd::Instruction_package instr_pkg(exec_component, next_pc, m_CPU_attribute);

        cmd(instr_pkg);

        if (instr_pkg.except == nRISC_V_cmd::execution_exception::ecall)
        {
            auto val = nSyscall::Do_syscall(exec_component, m_program_mdata);
            reg_file.gp_regs[nRISC_V_cpu_spec::gp_reg_abi_name::a0] = val;
        }

        // x0 is hard wired 0
        reg_file.gp_regs[nRISC_V_cpu_spec::RV_reg_file::x0] = 0;

        CHECK_ERROR(instr_pkg.regs.gp_regs[(uint32_t) nRISC_V_cpu_spec::gp_reg_abi_name::sp] % m_program_mdata.stack_pointer_alignment == 0);
        reg_file.pc = instr_pkg.next_pc;

        if (instr_pkg.except == nRISC_V_cmd::execution_exception::finish)
            break;
    }
}

