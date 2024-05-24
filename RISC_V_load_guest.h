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
    struct Loaded_guest_configure;

    bool Load_Elf_header(FILE *file_stream, Elf64_Ehdr *hdr);
    
    std::size_t Get_least_memory_needed(const char *program_name);
    
    void Load_guest_program(Loaded_guest_configure &config);

    struct Loaded_guest_configure
    {
        Loaded_guest_configure(std::string name, 
                               nProgram_mdata::Program_mdata_t *program_mdata, 
                               std::size_t runtime_data_space, 
                               std::size_t reserved_space,
                               std::unique_ptr<char[]> *mem) 
        : program_name(name), 
          program_mdata(*program_mdata),
          runtime_data_space(runtime_data_space), 
          reserved_space(reserved_space), 
          mem(*mem)
        {

        }

        const std::string &program_name;
        nProgram_mdata::Program_mdata_t &program_mdata;
        std::size_t runtime_data_space, reserved_space;
        std::unique_ptr<char[]> &mem;
    };

    
}