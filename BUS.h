#pragma once
#include <string.h>
#include <memory>
#include <assert.h>
#include "RISC_V_cpu_spec.h"

class BUS
{
public:
    BUS(std::unique_ptr<char[]> mem, const nRISC_V_cpu_spec::CPU_Attribute &CPU_attribute) : m_CPU_archietecture(CPU_attribute), m_mem(std::move(mem)){};
    ~BUS() = default;

    // memcpy to the target
    void Load_data(void *dst, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size);

    void SB(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr);
    void SH(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr);
    void SW(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr);
    void SD(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr);
    void* Get_raw_ptr(nRISC_V_cpu_spec::RISC_V_Addr_t addr) {return &m_mem.get()[addr - m_CPU_archietecture.base_addr];}
    void Fetch_instruction(const nRISC_V_cpu_spec::RV64_Regster_file &reg_file, nRISC_V_cpu_spec::RISC_V_Instr_t *dst);
    nUtil::eEndian endian() const { return m_CPU_archietecture.endian; }

private:
    bool Verify_addr(nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size) const;
    void Store_data(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size);
    const nRISC_V_cpu_spec::CPU_Attribute &m_CPU_archietecture;
    std::unique_ptr<char[]> m_mem;
};

inline void BUS::Load_data(void *dst, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size)
{  
    CHECK_ERROR(Verify_addr(addr, size) == true);
    memcpy(dst, Get_raw_ptr(addr), size);
}

inline void BUS::Store_data(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size)
{
    if (m_CPU_archietecture.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(src);
            
    memcpy(Get_raw_ptr(addr), &src, size);
}

inline void BUS::SB(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr)
{   
    CHECK_ERROR(Verify_addr(addr, 1) == true);

    Store_data(src, addr, 1);
}

inline void BUS::SH(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr)
{
    CHECK_ERROR(Verify_addr(addr, 2) == true);

    Store_data(src, addr, 2);
}

inline void BUS::SW(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr)
{
    CHECK_ERROR(Verify_addr(addr, 4) == true);

    Store_data(src, addr, 4);
}

inline void BUS::SD(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr)
{   
    CHECK_ERROR(Verify_addr(addr, 8) == true);

    Store_data(src, addr, 8);
}

inline void BUS::Fetch_instruction(const nRISC_V_cpu_spec::RV64_Regster_file &reg_file, nRISC_V_cpu_spec::RISC_V_Instr_t *dst)
{
    CHECK_ERROR(Verify_addr(reg_file.pc, sizeof(*dst)) == true);
    memcpy(dst, Get_raw_ptr(reg_file.pc), sizeof(*dst));

    if (m_CPU_archietecture.endian != nUtil::gHOST_ENDIAN)
        nUtil::Swap_endian(*dst);
}

