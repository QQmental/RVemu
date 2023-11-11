#include <string>
#include <assert.h>
#include "BUS.h"

void BUS::Load_data(char *dst, RISC_V_Addr_t addr, std::size_t size)
{  
    assert(addr >= m_CPU_archietecture.base_addr);
    assert(addr + size <= m_CPU_archietecture.highest_addr);
    memcpy(dst, &m_mem.get()[addr - m_CPU_archietecture.base_addr], size);
}

void BUS::Store_data(const char *src, RISC_V_Addr_t addr, std::size_t size)
{
    assert(addr >= m_CPU_archietecture.base_addr);
    assert(addr + size <= m_CPU_archietecture.highest_addr);
    memcpy(&m_mem.get()[addr - m_CPU_archietecture.base_addr], src, size);
}


void BUS::Fetch_instruction(const RV64I_regster_file &reg_file, RISC_V_Instr_t *dst)
{
    assert(reg_file.pc >= m_CPU_archietecture.base_addr);
    *dst = *reinterpret_cast<RISC_V_Instr_t*>(m_mem.get()[reg_file.pc - m_CPU_archietecture.base_addr]) ;
}