#include "crc_hash.h"

using namespace Borealis::Types;

namespace Borealis::Math
{
	BOREALIS_API uint64 HashValue(const char* data, uint64 size)
	{
		uint64 crc = 0;

		for (uint64 i = 0; i < size; i++)
			crc = CRC64_TABLE_REF[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);

		uint64 tmp = crc;
		crc = 0;
		for (int i = 0; i < 64; i++) {
			crc = (crc << 1) | (tmp & 1);
			tmp >>= 1;
		}

		return crc;
	}

	uint64 ReverseBits(uint64 bits, uint64 size)
	{
		uint64 rBits = 0;

		// TODO: Check if anything right here can be processed in parallel using SSE
		for (short i = 0; i < size; i++)
		{
			if ((bits & (1 << i)) != 0)
			{
				rBits |= (1 << (63 - i));
			}
		}

		return rBits;
	}
}