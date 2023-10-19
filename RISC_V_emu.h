#pragma once

#include "elf.h"
#include "RISC_V_spec.h"
#include <memory>
#include <string>

struct Program_mdata_t
{
    Elf64_Addr segment_base, brk_addr, pc;
};

class RISC_V_Instruction_map;

class RISC_V_Emulator
{
public:
    RISC_V_Emulator(const std::string &program_name);
    RISC_V_Emulator(const RISC_V_Emulator &src) = delete;

    RISC_V_Emulator& operator=(const RISC_V_Emulator &src) = delete;
    ~RISC_V_Emulator() = default;

    const Program_mdata_t& program_mdata() const {return m_progam_mdata;}
private:
    Program_mdata_t m_progam_mdata;
    RISC_V_Attributes m_RISC_V_attributes;
    std::unique_ptr<char []> m_mem;
    RISC_V_Instruction_map *m_instruction_map;
};

