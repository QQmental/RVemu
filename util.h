#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#define FATALF(fmt, ...) (fprintf(stderr, "fatal: %s:%d " fmt, __FILE__, __LINE__, __VA_ARGS__), abort())


#ifdef NDEBUG
    #define CHECK_ERROR(statement) ((void)0)
#else
    /*if the statement is false, it's an error*/
    #define CHECK_ERROR(statement) \
    if ((statement) == false) \
        FATALF("%s", #statement);
#endif

namespace nUtil
{

inline void TODO(const char *msg) {(fprintf(stderr, "warning: %s:%d [TODO] %s\n", __FILE__, __LINE__, msg)); abort();}
inline void FATAL(const char *msg) {FATALF("%s", msg);}


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
    using num_t = std::decay_t<integer_t>;

    static_assert(std::is_integral<num_t>::value == true, "num should be an integer\n");

    if constexpr(std::is_signed<num_t>::value == true)
    {
        using uint_type = typename std::make_unsigned<num_t>::type;
        uint_type tmp = num;
        Swap_endian(tmp);
        num = tmp;
        return;
    }
    else
        static_assert(sizeof(num) < 0, "unspecialized Swap_endian(unsigned &num) is called\n");
}

template<>
inline void Swap_endian(uint8_t &num)
{
    return ;
}

template<>
inline void Swap_endian(uint16_t &num)
{
   uint8_t upper = num >> 8, lower = num & 0xFF;
   Swap_endian(upper);
   Swap_endian(lower);
   num = (((uint16_t)lower) << 8) | upper;
}

template<>
inline void Swap_endian(uint32_t &num)
{
   uint16_t upper = num >> 16, lower = num & 0xFFFF;
   Swap_endian(upper);
   Swap_endian(lower);
   num = (((uint32_t)lower) << 16) | upper;
}

template<>
inline void Swap_endian(uint64_t &num)
{
   uint32_t upper = num >> 32, lower = num & 0xFFFFFFFF;
   Swap_endian(upper);
   Swap_endian(lower);
   num = (((uint64_t)lower) << 32) | upper;
}

}
