#pragma once
#include "RISC_V_cpu_spec.h"
#include <memory>



class BUS
{
public:

    
    BUS(std::unique_ptr<char[]> mem, const nRISC_V_cpu_spec::CPU_Attribute &CPU_attribute) : m_CPU_archietecture(CPU_attribute), m_mem(std::move(mem)){};
    ~BUS() = default;

    // memcpy to the target
    void Load_data(char *dst, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size);
    void Store_data(const char *src, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size);

    void Fetch_instruction(const nRISC_V_cpu_spec::RV64I_regster_file &reg_file, nRISC_V_cpu_spec::RISC_V_Instr_t *dst);
    nUtil::eEndian endian() const {return m_CPU_archietecture.endian;}

private:
    const nRISC_V_cpu_spec::CPU_Attribute &m_CPU_archietecture;
    std::unique_ptr<char[]> m_mem;    
};

