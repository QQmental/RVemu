#include "RISC_V_emu.h"

int main(int argc, char *argv[])
{
    for(int i = 0 ; i < argc ; i++)
        printf("%s\n",argv[i]);
    RISC_V_Emulator emu(std::string(argv[1]) , argc-1, const_cast<const char**>(argv+1));

    emu.start(); 
}