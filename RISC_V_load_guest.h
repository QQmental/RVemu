#pragma once

#include <string>
#include <string.h>
#include <stdint.h>

namespace nRISC_V_cpu_spec
{
struct CPU_Attribute;
struct RISC_V_Attributes;
}

struct Program_mdata_t;


namespace nRISC_V_load_guest
{
    void Init_basic_CPU_attributes(std::string program_name, nRISC_V_cpu_spec::CPU_Attribute &CPU_attribute);
    void Init_guest_segment_mapping(std::string program_name, Program_mdata_t &program_mdata, char* mem, uint8_t *&sh_RISC_V_attr);
    void Init_guest_RISC_V_attributes(nRISC_V_cpu_spec::RISC_V_Attributes &attr, const uint8_t *RISC_V_attributes_section);
}