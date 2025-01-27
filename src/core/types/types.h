#pragma once
#include "../../borealis_engine.h"
#include <cstdint>

namespace Borealis::Types
{	
	
#ifdef BOREALIS_WIN

		typedef int_fast8_t		int8;
		typedef short			int16;
		typedef int_fast32_t	int32;
		typedef int_fast64_t	int64;
		
		typedef uint_fast8_t	uint8;
		typedef unsigned short	uint16;
		typedef uint_fast32_t	uint32;
		typedef uint_fast64_t	uint64;
		
#elif BOREALIS_LINUX

		// @TODO: Adapt for Linux
		typedef int_fast8_t		int8;
		typedef short			int16;
		typedef int				int32;
		typedef int_fast64_t	int64;

		typedef uint_fast8_t	uint8;
		typedef unsigned short	uint16;
		typedef unsigned int	uint32;
		typedef uint_fast64_t	uint64;

#elif BOREALIS_OSX

		// @TODO: Adapt for OSX
		typedef int_fast8_t		int8;
		typedef int_fast16_t	int16;
		typedef int_fast32_t	int32;
		typedef int_fast64_t	int64;

		typedef uint_fast8_t	uint8;
		typedef uint_fast16_t	uint16;
		typedef uint_fast32_t	uint32;
		typedef uint_fast64_t	uint64;

#endif
}
