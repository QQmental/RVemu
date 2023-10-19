#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <memory>

#include "elf.h"
#include "RISC_V_emu.h"








int main(int argc, char *argv[])
{
    //Program_mdata_t program_mdata;
    
    //auto mem = std::unique_ptr<char[]>(new char[1<<20]);

    //Init_emulated_program("./test", program_mdata, mem);


    RISC_V_Emulator emu("test");

    auto program_mdata = emu.program_mdata();
    
    std::cout << std::hex << program_mdata.segment_base << " " << std::hex << program_mdata.brk_addr << " " << "hi\n";

    for(int i = 0 ; i < argc ; i++)
        printf("%s\n",argv[i]);
}