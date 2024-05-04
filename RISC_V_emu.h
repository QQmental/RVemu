#pragma once

#include "program_mdata.h"
#include <string>
#include <memory>

class RISC_V_Instruction_map;

class RISC_V_Emulator
{
public:
    RISC_V_Emulator(const std::string &program_name, int argc, const char **argv);
    RISC_V_Emulator(const RISC_V_Emulator &src) = delete;

    RISC_V_Emulator& operator=(const RISC_V_Emulator &src) = delete;
    ~RISC_V_Emulator();

    void start();
    const nProgram_mdata::Program_mdata_t& program_mdata() const {return m_program_mdata;}
    const nRISC_V_cpu_spec::CPU_Attribute& CPU_attribute() const {return m_CPU_archietecture;}

private:
    nProgram_mdata::Program_mdata_t m_program_mdata;
    nRISC_V_cpu_spec::CPU_Attribute m_CPU_archietecture;
    std::unique_ptr<char []> m_mem;
    std::unique_ptr<RISC_V_Instruction_map> m_instruction_map;
    const char **m_argv;
    int m_argc;

};

