
#include "helper.h"

inline bool is_printable(uint8_t b)
{
    // see https://de.wikipedia.org/wiki/American_Standard_Code_for_Information_Interchange
    return (b >= 0x20) && (b <= 0x7E);
}

inline void word2binary(uint32_t value, char buffer[])
{
    // 32 bits
    for (int i=31; i>=0; i--)
    {
        // extract LSB
        uint8_t bit = value & 0x00000001;
        // convert to ASCII '0' or '1'
        buffer[i] = 0x30 + bit;
        // shift right to get next higher bit
        value >>= 1;
    }
    // append string terminator
    buffer[32] = '\0';
}

inline char nibble2hex(uint8_t n)
{
    return n < 10 ? '0'+n : 'A'+n-10;
}

inline void byte2hex(uint8_t b, char buffer[], bool prefix)
{
    // prefix
    buffer[0] = '0';
    buffer[1] = 'x';

    // hex
    buffer[prefix*2]   = nibble2hex(b >> 4);
    buffer[prefix*2+1] = nibble2hex(b & 0x0F);

    // append string terminator
    buffer[2 + prefix*2] = '\0';
}
