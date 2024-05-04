#pragma once

#include <string>
#include <string.h>
#include <stdint.h>
#include <memory>

namespace nRISC_V_cpu_spec
{
struct CPU_Attribute;
struct RISC_V_Attributes;
}

namespace nProgram_mdata
{
struct Program_mdata_t;
}

struct elf64_hdr;
typedef struct elf64_hdr Elf64_Ehdr;



namespace nRISC_V_load_guest
{
    // an array of uint8_t[] is allocted to save RISC_V_attr
    // highest_addr is set to be the highest addr among those loaded segment
    // a piece of memory is allocated for 'sh_RISC_V_attr' when a segment of type 'PT_RISC_V_ATTRIBUTES' exists.
    void Init_guest_segment_mapping(std::string program_name, nProgram_mdata::Program_mdata_t &program_mdata, char* mem,  std::unique_ptr<uint8_t[]> &sh_RISC_V_attr);
    void Init_guest_RISC_V_attributes(nRISC_V_cpu_spec::RISC_V_Attributes &attr, const uint8_t *RISC_V_attributes_section);
    bool Load_Elf_header(FILE *file_stream, Elf64_Ehdr *hdr);
}