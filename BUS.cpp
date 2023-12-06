#include <string>
#include <string.h>
#include <assert.h>
#include "BUS.h"


using namespace nRISC_V_cpu_spec;

void BUS::Load_data(char *dst, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size)
{  
    assert(addr >= m_CPU_archietecture.base_addr);
    assert(addr + size <= m_CPU_archietecture.highest_addr);
    memcpy(dst, &m_mem.get()[addr - m_CPU_archietecture.base_addr], size);
}

void BUS::Store_data(const char *src, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size)
{
    assert(addr >= m_CPU_archietecture.base_addr);
    assert(addr + size <= m_CPU_archietecture.highest_addr);
    memcpy(&m_mem.get()[addr - m_CPU_archietecture.base_addr], src, size);
}


void BUS::Fetch_instruction(const nRISC_V_cpu_spec::RV64_Regster_file &reg_file, nRISC_V_cpu_spec::RISC_V_Instr_t *dst)
{
    assert(reg_file.pc >= m_CPU_archietecture.base_addr);
    assert(reg_file.pc + sizeof(*dst) <= m_CPU_archietecture.highest_addr);
    memcpy(dst, &m_mem.get()[(reg_file.pc - m_CPU_archietecture.base_addr)], sizeof(*dst));
    //*dst = *reinterpret_cast<nRISC_V_cpu_spec::RISC_V_Instr_t*>(m_mem.get() + (reg_file.pc - m_CPU_archietecture.base_addr)) ;
}