#pragma once

#include "elf.h"
#include "RISC_V_cpu_spec.h"
#include <memory>
#include <string>

struct Program_mdata_t
{
    Elf64_Addr segment_base, brk_addr, entry_point;

    Elf64_Addr stack_pointer_alignment;
};

class RISC_V_Instruction_map;

class RISC_V_Emulator
{
public:
    RISC_V_Emulator(const std::string &program_name);
    RISC_V_Emulator(const RISC_V_Emulator &src) = delete;

    RISC_V_Emulator& operator=(const RISC_V_Emulator &src) = delete;
    ~RISC_V_Emulator();

    void start();
    const Program_mdata_t& program_mdata() const {return m_program_mdata;}
    const CPU_Attribute& CPU_attribute() const {return m_CPU_Archietecture;}

private:
    Program_mdata_t m_program_mdata;
    CPU_Attribute m_CPU_Archietecture;
    std::unique_ptr<char []> m_mem;
    std::unique_ptr<RISC_V_Instruction_map> m_instruction_map;

};

