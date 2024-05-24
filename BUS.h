#pragma once
#include <string.h>
#include <memory>
#include <assert.h>
#include "RISC_V_cpu_spec.h"
#include "program_mdata.h"

class BUS
{
public:
    BUS(std::unique_ptr<char[]> mem, nProgram_mdata::Program_mdata_t &program_mdata) : m_program_mdata(program_mdata), m_mem(std::move(mem)){};
    ~BUS() = default;

    // memcpy to the target
    void Load_data(void *dst, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size);

    void SB(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr);
    void SH(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr);
    void SW(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr);
    void SD(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr);
    void* Get_raw_ptr(nRISC_V_cpu_spec::RISC_V_Addr_t addr) 
    {
        CHECK_ERROR(Verify_addr(addr, 1) == true);
        return &m_mem.get()[addr - m_program_mdata.segment_base];
    }
    void Fetch_instruction(const nRISC_V_cpu_spec::RV64_Regster_file &reg_file, nRISC_V_cpu_spec::RISC_V_Instr_t *dst);
    nUtil::eEndian endian() const { return m_program_mdata.CPU_attributes.endian; }

private:
    bool Verify_addr(nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size) const;
    void Store_data(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size);
    nProgram_mdata::Program_mdata_t &m_program_mdata;
    std::unique_ptr<char[]> m_mem;
};

inline void BUS::Load_data(void *dst, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size)
{  
    CHECK_ERROR(Verify_addr(addr, size) == true);

    CHECK_ERROR((   addr + size > m_program_mdata.brk_addr 
                 && addr + size < *m_program_mdata.stack_top) == false);

    CHECK_ERROR((   addr > m_program_mdata.brk_addr 
                 && addr < *m_program_mdata.stack_top) == false);


    memcpy(dst, Get_raw_ptr(addr), size);
}

inline void BUS::Store_data(nRISC_V_cpu_spec::RV_int_reg_t src, nRISC_V_cpu_spec::RISC_V_Addr_t addr, std::size_t size)
{
    CHECK_ERROR((   addr + size > m_program_mdata.brk_addr 
                 && addr + size < *m_program_mdata.stack_top) == false);

    CHECK_ERROR((   addr > m_program_mdata.brk_addr 
                 && addr < *m_program_mdata.stack_top) == false);

    // store low bits to rd reg
    if (nUtil::gHOST_ENDIAN != nUtil::eEndian::little_endian)
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

    // TODO: if no c extension support, it should be multiple of 4 only
    CHECK_ERROR(reg_file.pc % 4 == 0 || reg_file.pc % 2 == 0);

    memcpy(dst, Get_raw_ptr(reg_file.pc), sizeof(*dst));

    if (nUtil::gHOST_ENDIAN  != nUtil::little_endian)
        nUtil::Swap_endian(*dst);
}

