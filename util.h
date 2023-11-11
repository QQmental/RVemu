#pragma once
#include <stdint.h>
#include <utility>

namespace nUtil
{
enum eEndian : int
{
    little_endian = 0,
    big_endian,
    other
};

static inline eEndian gHOST_ENDIAN = []()
{
    uint32_t checking_value = 0x12345678;
 
    uint8_t *ptr = (uint8_t*)&checking_value;

    if (ptr[0] == 0x12 && ptr[3] == 0x78)
        return eEndian::big_endian;
    else if (ptr[0] == 0x78 && ptr[3] == 0x12)
        return eEndian::little_endian;
    return eEndian::other;    
}();

template<typename integer_t>
void Swap_endian(integer_t &num)
{
    auto t = num;

    static_assert(std::is_integral<decltype(t)>::value == true, "num should be an integer\n");

    if constexpr(std::is_signed<decltype(t)>::value == true)
    {
        using uint_type = typename std::make_unsigned<decltype(t)>::type;
        Swap_endian(*reinterpret_cast<uint_type*>(&num));
        return;
    }
    else
        static_assert(0, "exception: unspecialized Swap_endian(unsigned &num) is called \n");
    

}

template<>
inline void Swap_endian(uint8_t &num)
{
    return ;
}

template<>
inline void Swap_endian(uint16_t &num)
{
   uint8_t *ptr = reinterpret_cast<uint8_t*>(&num);
   std::swap(ptr[0], ptr[1]);
   Swap_endian(ptr[0]);
   Swap_endian(ptr[1]);
}

template<>
inline void Swap_endian(uint32_t &num)
{
   uint16_t *ptr = reinterpret_cast<uint16_t*>(&num);
   std::swap(ptr[0], ptr[1]);
   Swap_endian(ptr[0]);
   Swap_endian(ptr[1]);
}

template<>
inline void Swap_endian(uint64_t &num)
{
   uint32_t *ptr = reinterpret_cast<uint32_t*>(&num);
   std::swap(ptr[0], ptr[1]);
   Swap_endian(ptr[0]);
   Swap_endian(ptr[1]);
}

};
