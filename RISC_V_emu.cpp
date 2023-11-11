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

constexpr RISC_V_double_word_t gDEFAULT_START_STACK_POINTER = 0xc800'0000'0000'0000;

using instr_cmd_t = void (*)(nRISC_V_cmd::Instruction_package&);

enum eCPU_state : uint16_t
{
    stop = 0,
    ready = 1,
    running
};

class RISC_V_Instruction_map
{
public:
    std::unordered_map<RISC_V_Instr_t, instr_cmd_t> m_map;

};


struct CPU_Component
{
    BUS bus;
    RV64I_regster_file reg_file;
    RV_Instr_component RV_instr_component;
    eCPU_state CPU_state;
};

static bool Processing_instruction(const RISC_V_Instr_t &instruction, RISC_V_Instr_t *mask_dst, RV_Instr_component &component);



RISC_V_Emulator::RISC_V_Emulator(const std::string &program_name)
{
    uint8_t *sh_RISC_V_attr {};
    
    m_CPU_Archietecture = {};

    m_mem = std::unique_ptr<char[]>(new char[1<<20]);

    nRISC_V_load_guest::Init_basic_CPU_attributes(program_name, m_CPU_Archietecture);
    nRISC_V_load_guest::Init_guest_segment_mapping(program_name, m_program_mdata, m_mem.get(), sh_RISC_V_attr);
    nRISC_V_load_guest::Init_guest_RISC_V_attributes(m_CPU_Archietecture.RISC_V_attributes, sh_RISC_V_attr);
    
    if (m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_stack_align.first == true)
        m_program_mdata.stack_pointer_alignment = m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_stack_align.second;
    
    

    printf("%s %u %u %u %u\n", m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_arch.second.c_str(), \
    m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_stack_align.second,\
    m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_unaligned_access.second, \
    m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_atomic_abi.second, \
    m_CPU_Archietecture.RISC_V_attributes.Tag_RISCV_x3_reg_usage.second);
    


    m_instruction_map = std::make_unique<RISC_V_Instruction_map>();



    delete[] sh_RISC_V_attr;
}

RISC_V_Emulator::~RISC_V_Emulator()
{
    
}

// decompose instruction into components and mask instruction for map searching
// return false if the instruction has invalid opcode
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

             if (nRISC_V_decompose::funct3(instruction) == 0b001 || nRISC_V_decompose::funct3(instruction) == 0b101)
                *mask_dst = instruction & RISC_V_Instr_t(0b111111111111'00000'111'00000'1111111);
             else
                *mask_dst = instruction & RISC_V_Instr_t(0b000000000000'00000'111'00000'1111111);                
        break;

        case 0b0011011:
             nRISC_V_decompose::Decompose_Itype_instruction(component, instruction);    

             if (nRISC_V_decompose::funct3(instruction) == 0b000)
                 *mask_dst = instruction & RISC_V_Instr_t(0b000000000000'00000'111'00000'1111111);   
             else
                *mask_dst = instruction & RISC_V_Instr_t(0b111111111111'00000'111'00000'1111111);
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
    nRISC_V_cmd::RV_reg_file reg_file = {};
    reg_file.pc = m_program_mdata.entry_point;
    reg_file.gp_regs[nRISC_V_cmd::RV_reg_file::x2] = gDEFAULT_START_STACK_POINTER;

    RV_Instr_component RV_instr_component = {};

    BUS bus(std::move(m_mem), m_CPU_Archietecture);

    RISC_V_Instr_t instruction, masked_instruction;
    
    while(1)
    {    
        bus.Fetch_instruction(reg_file, &instruction);
        
        if (m_CPU_Archietecture.endian != nUtil::gHOST_ENDIAN)
            nUtil::Swap_endian(instruction);

        /* the lowest 2 bits of a compressed instruction is 0b11*/
        if ((nRISC_V_decompose::opcode(instruction) & 0b11) != 0b11)
        {

        }

        bool opcode_type_status = Processing_instruction(instruction, &masked_instruction, RV_instr_component);

        assert(opcode_type_status == true);

        nRISC_V_cmd::Instruction_package instr_pkg(reg_file, 
                                                   bus, 
                                                   RV_instr_component, 
                                                   m_CPU_Archietecture, 
                                                   instruction);

        auto it_instr_cmd = m_instruction_map->m_map.find(masked_instruction);
        
        assert(it_instr_cmd != m_instruction_map->m_map.end());

        it_instr_cmd->second(instr_pkg);

        if (instr_pkg.except == nRISC_V_cmd::execution_exception::finish)
            break;
    }
}

