#pragma once

#include "RISC_V_emu.h"
#include "RISC_V_cmd.h"

namespace nSyscall
{
    uint64_t Do_syscall(nRISC_V_cmd::Exec_component &exec_compnent, nProgram_mdata::Program_mdata_t &program_mdata);
}