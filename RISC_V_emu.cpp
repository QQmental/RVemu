#include <iostream>
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

using nRISC_V_cpu_spec::RISC_V_Instr_t;
using nRISC_V_cpu_spec::RV_Instr_component;

constexpr nRISC_V_cpu_spec::RISC_V_double_word_t gDEFAULT_START_STACK_POINTER = 0x0000'0000'0200'0000;

// reserve 4k bytes between START_STACK_POINTER and USER_HIGHESET_ADDR
constexpr nRISC_V_cpu_spec::RISC_V_double_word_t
gDEFAULT_USER_HIGHESET_ADDR = gDEFAULT_START_STACK_POINTER + (1<<12);

enum eCPU_state : uint16_t
{
    stop = 0,
    ready = 1,
    running
};


class RISC_V_Instruction_map
{
public:
    struct Command_attribute
    {
        nRISC_V_cmd::instr_cmd_t cmd;
        nRISC_V_decompose::eOpecode_type op_type;
        std::string name;
    };
    
    // return true if the cmd hasn't been inserted before Regist_cmd is called.
    bool Regist_cmd(RISC_V_Instr_t cmd_mask, nRISC_V_cmd::instr_cmd_t cmd);
    bool Regist_cmd(RISC_V_Instr_t cmd_mask, const RISC_V_Instruction_map::Command_attribute &attr);
    
    // a map for non-compressed instructions
    std::unordered_map<RISC_V_Instr_t, nRISC_V_cmd::instr_cmd_t> m_map;

};

bool RISC_V_Instruction_map::Regist_cmd(RISC_V_Instr_t cmd_mask, nRISC_V_cmd::instr_cmd_t cmd)
{
    nRISC_V_decompose::eOpecode_type garbage;
    
    RISC_V_Instruction_map::Command_attribute attr;
    attr.cmd = cmd;
    attr.op_type = garbage;
    attr.name = "";
    
    return Regist_cmd(cmd_mask, attr);
}

bool RISC_V_Instruction_map::Regist_cmd(RISC_V_Instr_t cmd_mask, const RISC_V_Instruction_map::Command_attribute &attr)
{
    auto f = m_map.emplace(cmd_mask, attr.cmd);

    return f.second == true;
}


static bool Processing_instruction(const RISC_V_Instr_t &instruction, RISC_V_Instr_t *mask_dst, RV_Instr_component &component);
static void Init_basic_CPU_attributes(std::string program_name, const Program_mdata_t &program_mdata, nRISC_V_cpu_spec::CPU_Attribute &CPU_attribute);
static void Regist_RVI_cmd(RISC_V_Instruction_map &);


static void Init_basic_CPU_attributes(std::string program_name, const Program_mdata_t &program_mdata, nRISC_V_cpu_spec::CPU_Attribute &CPU_attribute)
{
    Elf64_Ehdr hdr;

    auto file_stream = std::fopen(program_name.c_str(), "rb");

    assert(file_stream != nullptr);

    auto f = nRISC_V_load_guest::Load_Elf_header(file_stream, &hdr);
    assert(f == true);

    if (hdr.e_ident[EI_CLASS] == ELFCLASS64)
        CPU_attribute.xlen = 64;
    else if (hdr.e_ident[EI_CLASS] == ELFCLASS32)
        CPU_attribute.xlen = 32;

    if (hdr.e_ident[EI_DATA] == ELFDATA2LSB)
        CPU_attribute.endian = nUtil::eEndian::little_endian;
    else if(hdr.e_ident[EI_DATA] == ELFDATA2MSB)
        CPU_attribute.endian = nUtil::eEndian::big_endian;
    else if (hdr.e_ident[EI_DATA] == ELFCLASSNONE)
        CPU_attribute.endian = nUtil::eEndian::other;
    else
    {
        printf("undifined encoding %s %d\n", __func__, __LINE__);
        abort();
    }    

    CPU_attribute.highest_addr = program_mdata.highest_addr;
    CPU_attribute.base_addr = program_mdata.segment_base;
}

static void Regist_RVI_cmd(RISC_V_Instruction_map &map)
{
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

    REGIST_CMD(0b00000000000000000000'00000'0110111, LUI, U);
    REGIST_CMD(0b00000000000000000000'00000'0010111, AUIPC, U);

    REGIST_CMD(0b00000000000000000000'00000'1101111, JAL, J);
    
    REGIST_CMD(0b000000000000'00000'000'00000'1100111, JALR, I);
    REGIST_CMD(0b000000000000'00000'000'00000'0000111, LB,   I);
    REGIST_CMD(0b000000000000'00000'001'00000'0000111, LH,   I);
    REGIST_CMD(0b000000000000'00000'010'00000'0000111, LW,   I);
    REGIST_CMD(0b000000000000'00000'100'00000'0000111, LBU,  I);
    REGIST_CMD(0b000000000000'00000'101'00000'0000111, LHU,  I);                
    REGIST_CMD(0b000000000000'00000'000'00000'0010011, ADDI,  I); 
    REGIST_CMD(0b000000000000'00000'010'00000'0010011, SLTI,  I);
    REGIST_CMD(0b000000000000'00000'011'00000'0010011, SLTIU,  I);
    REGIST_CMD(0b000000000000'00000'100'00000'0010011, XORI,  I);
    REGIST_CMD(0b000000000000'00000'110'00000'0010011, ORI,  I);
    REGIST_CMD(0b000000000000'00000'111'00000'0010011, ANDI,  I);

    //RV64 SLLI, SRLI, SRAI share same format with those in RV32
    REGIST_CMD(0b000000000000'00000'001'00000'0010011, SLLI,  I);
    REGIST_CMD(0b000000000000'00000'101'00000'0010011, SRLI,  I);
    REGIST_CMD(0b010000000000'00000'101'00000'0010011, SRAI,  I);

    REGIST_CMD(0b000000000000'00000'000'00000'0001111, FENCE,  I);
    REGIST_CMD(0b000000000000'00000'000'00000'1110011, ECALL,  I);
    REGIST_CMD(0b000000000001'00000'000'00000'1110011, EBREAK,  I);

    //RV64I
    REGIST_CMD(0b000000000000'00000'110'00000'0000011, LWU,  I);
    REGIST_CMD(0b000000000000'00000'011'00000'0000011, LD,  I);
    REGIST_CMD(0b000000000000'00000'000'00000'0011011, ADDIW,  I);  
    REGIST_CMD(0b000000000000'00000'001'00000'0011011, SLLIW,  I);
    REGIST_CMD(0b000000000000'00000'101'00000'0011011, SRLIW,  I);
    REGIST_CMD(0b010000000000'00000'101'00000'0011011, SRAIW,  I);


    REGIST_CMD(0b0000000'00000'00000'000'00000'0110011, ADD,  R);    
    REGIST_CMD(0b0100000'00000'00000'000'00000'0110011, SUB,  R); 
    REGIST_CMD(0b0000000'00000'00000'001'00000'0110011, SLL,  R);
    REGIST_CMD(0b0000000'00000'00000'010'00000'0110011, SLT,  R);
    REGIST_CMD(0b0000000'00000'00000'011'00000'0110011, SLTU,  R);
    REGIST_CMD(0b0000000'00000'00000'100'00000'0110011, XOR,  R);
    REGIST_CMD(0b0000000'00000'00000'101'00000'0110011, SRL,  R);
    REGIST_CMD(0b0100000'00000'00000'101'00000'0110011, SRA,  R);
    REGIST_CMD(0b0000000'00000'00000'110'00000'0110011, OR,  R);
    REGIST_CMD(0b0000000'00000'00000'111'00000'0110011, AND,  R);

    //RV64I
    REGIST_CMD(0b0000000'00000'00000'000'00000'0111011, ADDW,  R);    
    REGIST_CMD(0b0100000'00000'00000'000'00000'0111011, SUBW,  R); 
    REGIST_CMD(0b0000000'00000'00000'001'00000'0111011, SLLW,  R);
    REGIST_CMD(0b0000000'00000'00000'101'00000'0111011, SRLW,  R);
    REGIST_CMD(0b0100000'00000'00000'101'00000'0111011, SRAW,  R);


    REGIST_CMD(0b0000000'00000'00000'000'00000'1100011, BEQ,  S);
    REGIST_CMD(0b0000000'00000'00000'001'00000'1100011, BNE,  S);
    REGIST_CMD(0b0000000'00000'00000'100'00000'1100011, BLT,  S);
    REGIST_CMD(0b0000000'00000'00000'101'00000'1100011, BGE,  S);
    REGIST_CMD(0b0000000'00000'00000'110'00000'1100011, BLTU, S);
    REGIST_CMD(0b0000000'00000'00000'111'00000'1100011, BGEU, S);
    REGIST_CMD(0b0000000'00000'00000'000'00000'0100011, SB,   S);
    REGIST_CMD(0b0000000'00000'00000'001'00000'0100011, SH,   S);
    REGIST_CMD(0b0000000'00000'00000'010'00000'0100011, SW,   S);    

    //RV64I
    REGIST_CMD(0b0000000'00000'00000'011'00000'0100011, SD,   S);

    #undef REGIST_CMD
}

RISC_V_Emulator::RISC_V_Emulator(const std::string &program_name)
{
    uint8_t *sh_RISC_V_attr {};
    
    m_CPU_Archietecture = {};

    m_mem = std::unique_ptr<char[]>(new char[gDEFAULT_USER_HIGHESET_ADDR]);

    nRISC_V_load_guest::Init_guest_segment_mapping(program_name, m_program_mdata, m_mem.get(), sh_RISC_V_attr);
    nRISC_V_load_guest::Init_guest_RISC_V_attributes(m_CPU_Archietecture.RISC_V_attributes, sh_RISC_V_attr);
    
    if (m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_stack_align.first == true)
        m_program_mdata.stack_pointer_alignment = m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_stack_align.second;
    else
        m_program_mdata.stack_pointer_alignment = 16;

    printf("%s %u %u %u %u\n", m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_arch.second.c_str(), \
    m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_stack_align.second,\
    m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_unaligned_access.second, \
    m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_atomic_abi.second, \
    m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_x3_reg_usage.second);
    
    if (m_program_mdata.highest_addr > gDEFAULT_USER_HIGHESET_ADDR)
        nUtil::FATAL("user addr is not high enough in guest\n");

    m_program_mdata.highest_addr = gDEFAULT_USER_HIGHESET_ADDR;

    Init_basic_CPU_attributes(program_name, m_program_mdata, m_CPU_Archietecture);

    m_instruction_map = std::make_unique<RISC_V_Instruction_map>();

    Regist_RVI_cmd(*m_instruction_map.get());
    
    delete[] sh_RISC_V_attr;
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
            nRISC_V_decompose::Decompose_Rtype_instruction(component, instruction);
            *mask_dst = instruction & RISC_V_Instr_t(0b1111111'00000'00000'111'00000'1111111);
        break;

        case 0b0000011:
            nRISC_V_decompose::Decompose_Itype_instruction(component, instruction);
            *mask_dst = instruction & RISC_V_Instr_t(0b000000000000'00000'111'00000'1111111);
        break;

        case 0b0010011:
            nRISC_V_decompose::Decompose_Itype_instruction(component, instruction);

            // SLLI, SRLI, SRAI ... bit 30 is needed for distinguishing between SR and SL
            if (nRISC_V_decompose::funct3(instruction) == 0b001 || nRISC_V_decompose::funct3(instruction) == 0b101)
               *mask_dst = instruction & RISC_V_Instr_t(0b0100000'00000'00000'111'00000'1111111);
            // ADDI, SLTI ...
            else
               *mask_dst = instruction & RISC_V_Instr_t(0b000000000000'00000'111'00000'1111111);                
        break;

        case 0b0011011:
            nRISC_V_decompose::Decompose_Itype_instruction(component, instruction);    

            if (nRISC_V_decompose::funct3(instruction) == 0b000)
                *mask_dst = instruction & RISC_V_Instr_t(0b000000000000'00000'111'00000'1111111);
            else // SLLIW, SRLIW, SRAIW ... bit 30 is needed for distinguishing between SR and SL  
               *mask_dst = instruction & RISC_V_Instr_t(0b0100000'00000'00000'111'00000'1111111);
        break;

        case 0b0001111:
             nRISC_V_decompose::Decompose_Itype_instruction(component, instruction);
             *mask_dst = instruction & RISC_V_Instr_t(0b000000000000'00000'111'00000'1111111);
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
             nRISC_V_decompose::Decompose_Utype_instruction(component, instruction);
             *mask_dst = instruction & RISC_V_Instr_t(0b00000000000000000000'00000'1111111);                
        break;

        case 0b0010111:
             nRISC_V_decompose::Decompose_Utype_instruction(component, instruction);
             *mask_dst = instruction & RISC_V_Instr_t(0b00000000000000000000'00000'1111111);                
        break;

        case 0b1101111:
             nRISC_V_decompose::Decompose_Jtype_instruction(component, instruction);
             *mask_dst = instruction & RISC_V_Instr_t(0b00000000000000000000'00000'1111111); 
        break;

        case 0b0111011:
             nRISC_V_decompose::Decompose_Rtype_instruction(component, instruction);
             *mask_dst = instruction & RISC_V_Instr_t(0b1111111'00000'00000'111'00000'1111111);
        break;

        default:
            opcode_type_status = false;
        break;
    }

    return opcode_type_status;
}

void RISC_V_Emulator::start()
{
    nRISC_V_cpu_spec::RV_reg_file reg_file = {};
    reg_file.pc = m_program_mdata.entry_point;
    reg_file.gp_regs[nRISC_V_cpu_spec::RV_reg_file::x2] = gDEFAULT_START_STACK_POINTER;

    nRISC_V_cpu_spec::RV_Instr_component RV_instr_component = {};

    BUS bus(std::move(m_mem), m_CPU_Archietecture);

    nRISC_V_cpu_spec::RISC_V_Instr_t instruction, masked_instruction;
    
    nRISC_V_cmd::Exec_component exec_component(reg_file, bus, RV_instr_component);

    nRISC_V_unzip_instr::Compressed_command_map cmprs_cmd_map;
    
    nRISC_V_cmd::instr_cmd_t cmd;

    while(1)
    {    
        bus.Fetch_instruction(reg_file, &instruction);
        
        if (m_CPU_Archietecture.endian != nUtil::gHOST_ENDIAN)
            nUtil::Swap_endian(instruction);

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

            auto it_instr_cmd = m_instruction_map->m_map.find(masked_instruction);

            printf("mask %ld\n",masked_instruction);
            CHECK_ERROR(it_instr_cmd != m_instruction_map->m_map.end());

            cmd = it_instr_cmd->second;
            
            next_pc = reg_file.pc + 4;
        }
        printf("test: %x %x\n",reg_file.pc , instruction);
        //06e000ef jal	101bc <main>, jump to main, then just stop
        if (instruction == 0x06e000ef)
            break;

        nRISC_V_cmd::Instruction_package instr_pkg(exec_component, next_pc, m_CPU_Archietecture);

        cmd(instr_pkg);

        // x0 is hard wired 0
        reg_file.gp_regs[nRISC_V_cpu_spec::RV_reg_file::x0] = 0;

        reg_file.pc = next_pc;

        if (instr_pkg.except == nRISC_V_cmd::execution_exception::finish)
            break;
    }
}

