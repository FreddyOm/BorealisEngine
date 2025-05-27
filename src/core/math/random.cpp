#include "./random.h"
#include <stdint.h>

using namespace Borealis::Types;

namespace Borealis::Math::Random
{
    static uint64 shuffle_table[] = { 0x632ec6da3cfd0aba, 0xd4a61297f0caafd7, 0xa9592612e03fc9aa, 0x39ab614509b1661e };
    static uint64 s[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

    static inline uint64_t rotl(const uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }

    uint64 Next32()
    {
        uint64 s1 = shuffle_table[0];
        uint64 s0 = shuffle_table[1];
        uint64 result = s0 + s1;
        shuffle_table[0] = s0;
        s1 ^= s1 << 23;
        shuffle_table[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
        return result;
    }

    uint64 Next64()
    {
        const uint64 result = rotl(s[0] + s[3], 23) + s[0];

        const uint64 t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;

        s[3] = rotl(s[3], 45);

        return result;
    }
}