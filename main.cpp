#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <memory>
#include <stddef.h>

#include "RISC_V_emu.h"








int main(int argc, char *argv[])
{
    RISC_V_Emulator emu("./test2");

    auto program_mdata = emu.program_mdata();
    
    

    std::cout << std::hex << program_mdata.segment_base << " " << std::hex << program_mdata.brk_addr << " " << "hi\n";

    std::cout << std::hex << program_mdata.entry_point << " " << \
    std::hex << emu.CPU_attribute().RISC_V_attributes.Tag_RISCV_stack_align.second << " " << "hi\n";

    printf("%u %d hi\n", emu.CPU_attribute().xlen, emu.CPU_attribute().endian);

    emu.start();
    
    for(int i = 0 ; i < argc ; i++)
        printf("%s\n",argv[i]);
}