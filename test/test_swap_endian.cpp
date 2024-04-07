#include <iostream>
#include <assert.h>
#include "../util.h"
#include "../RISC_V_decompose.h"

uint64_t func(uint16_t num) 
{
    nUtil::Swap_endian(num);
    return num;
}

int main()
{
    uint8_t x = 0xAB;
    nUtil::Swap_endian(x);
    assert(x == 0xAB);

    uint16_t y = 0xABCD;
    nUtil::Swap_endian(y);
    assert(y == 0xCDAB);    
    
    uint32_t z = 0xABCD0123;
    nUtil::Swap_endian(z);
    assert(z == 0x2301CDAB);

    uint64_t w = 0xABCD0123456789AB;
    nUtil::Swap_endian(w);
    assert(w == 0xAB8967452301CDAB);


    int64_t sw = 0xABCD0123456789AB;
    nUtil::Swap_endian(sw);
    assert(sw == 0xAB8967452301CDAB);


    nRISC_V_cpu_spec::RV_Instr_component result;
    nRISC_V_cpu_spec::RISC_V_Instr_t instr = 0x1234;
    nRISC_V_decompose::Decompose_Jtype_instruction(result, instr);

    return result.funct3;
}