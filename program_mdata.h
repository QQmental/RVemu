#pragma once
#include "RISC_V_cpu_spec.h"
#include "elf.h"

namespace nProgram_mdata
{
    
struct Program_mdata_t
{
    // stack_top is initialized when the regisiter file is created
    Elf64_Addr segment_base, brk_addr, *stack_top, entry_point;
    nRISC_V_cpu_spec::RISC_V_Addr_t highest_addr;
    Elf64_Addr stack_pointer_alignment;
    nRISC_V_cpu_spec::CPU_Attribute CPU_attributes;
};
}


