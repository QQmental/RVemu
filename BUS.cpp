#include <string>
#include <string.h>
#include <assert.h>
#include "BUS.h"


using namespace nRISC_V_cpu_spec;


struct BUS_cmd_package
{
    BUS_cmd_package(
    nRISC_V_cpu_spec::RISC_V_Addr_t addr, 
    nRISC_V_cpu_spec::RISC_V_Addr_t  min, 
    nRISC_V_cpu_spec::RISC_V_Addr_t max,
    std::size_t size) 
    : addr(addr), min(min), max(max), size(size)
    {

    }

    nRISC_V_cpu_spec::RISC_V_Addr_t addr, min, max;
    std::size_t size;
};





bool BUS::Verify_addr(nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size) const
{
    BUS_cmd_package pkg(addr, m_CPU_archietecture.base_addr, m_CPU_archietecture.highest_addr, size);
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



