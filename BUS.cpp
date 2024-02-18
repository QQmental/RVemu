#include <string>
#include <string.h>
#include <assert.h>
#include "BUS.h"


using namespace nRISC_V_cpu_spec;


struct BUS_cmd_package
{
    BUS_cmd_package(nRISC_V_cpu_spec::RV_int_reg_t &reg, 
    nRISC_V_cpu_spec::RISC_V_Addr_t addr, 
    nRISC_V_cpu_spec::RISC_V_Addr_t  min, 
    nRISC_V_cpu_spec::RISC_V_Addr_t max,
    std::size_t size) : reg(reg), addr(addr), min(min), max(max), size(size)
    {

    }
    nRISC_V_cpu_spec::RV_int_reg_t &reg;
    nRISC_V_cpu_spec::RISC_V_Addr_t addr, min, max;
    std::size_t size;
};

void BUS::Load_data(void *dst, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size)
{  
    assert(addr >= m_CPU_archietecture.base_addr);
    assert(addr + size <= m_CPU_archietecture.highest_addr);
    memcpy(dst, Get_raw_ptr(addr), size);
}

void BUS::Store_data(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size)
{
    if (m_CPU_archietecture.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(src);
            
    memcpy(Get_raw_ptr(addr), &src, size);
}

bool BUS::Verify_addr(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size) const
{
    BUS_cmd_package pkg(src, addr, m_CPU_archietecture.base_addr, m_CPU_archietecture.highest_addr, size);
    auto &attr = m_CPU_archietecture.RISC_V_attributes;

    if ((pkg.addr >= pkg.min) == false)
        return false;
    if ((pkg.addr + pkg.size <= pkg.max) == false)
        return false;
 
    if (attr.Tag_RISCV_unaligned_access.first == true && attr.Tag_RISCV_unaligned_access.second == 0)
    {
        if (pkg.addr % pkg.size != 0)
            return false;
    }
 
    return true;
}



void BUS::Fetch_instruction(const nRISC_V_cpu_spec::RV64_Regster_file &reg_file, nRISC_V_cpu_spec::RISC_V_Instr_t *dst)
{
    assert(reg_file.pc >= m_CPU_archietecture.base_addr);
    assert(reg_file.pc + sizeof(*dst) <= m_CPU_archietecture.highest_addr);
    memcpy(dst, Get_raw_ptr(reg_file.pc), sizeof(*dst));
}