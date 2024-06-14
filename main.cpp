#include "RISC_V_emu.h"

int main(int argc, char *argv[])
{
    RISC_V_Emulator emu(std::string(argv[1]) , argc-1, const_cast<const char**>(argv+1));

    emu.start(); 
}
