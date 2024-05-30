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
    const char *argv_arr[1] ;
    argv_arr[0] = "./test.elf";
    RISC_V_Emulator emu("./test.elf", 1, argv_arr);

    auto program_mdata = emu.program_mdata();

    emu.start();
}