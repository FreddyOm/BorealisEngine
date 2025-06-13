#pragma once
#include "../types/types.h"

namespace Borealis::Math
{
    extern const Borealis::Types::uint64 CRC64_TABLE_REF[256]; 

    BOREALIS_API constexpr Borealis::Types::uint64 CompileTimeHashValue(const char* data, Borealis::Types::uint64 size = 64)
    {
        Borealis::Types::uint64 crc = 0;

        for (Borealis::Types::uint64 i = 0; i < size; i++)
            crc = CRC64_TABLE_REF[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);

        Borealis::Types::uint64 tmp = crc;
        crc = 0;
        for (int i = 0; i < 64; i++) {
            crc = (crc << 1) | (tmp & 1);
            tmp >>= 1;
        }

        return crc;
    }
    
    BOREALIS_API Borealis::Types::uint64 HashValue(const char* data, Borealis::Types::uint64 size = 64);
    
    Borealis::Types::uint64 ReverseBits(Borealis::Types::uint64 bits, Borealis::Types::uint64 size);
}