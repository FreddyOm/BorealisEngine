#pragma once
#include "../../borealis_engine.h"
#include <cstdint>

namespace Borealis::Types
{	
	
#ifdef BOREALIS_WIN

		typedef int_fast8_t		Int8;
		typedef short			Int16;
		typedef int_fast32_t	Int32;
		typedef int_fast64_t	Int64;
		
		typedef uint_fast8_t	Uint8;
		typedef unsigned short	Uint16;
		typedef uint_fast32_t	Uint32;
		typedef uint_fast64_t	Uint64;
		
#elif BOREALIS_LINUX

		// @TODO: Adapt for Linux
		typedef int_fast8_t		Int8;
		typedef int_fast16_t	Int16;
		typedef int_fast32_t	Int32;
		typedef int_fast64_t	Int64;

		typedef uint_fast8_t	Uint8;
		typedef uint_fast16_t	Uint16;
		typedef uint_fast32_t	Uint32;
		typedef uint_fast64_t	Uint64;

#elif BOREALIS_OSX

		// @TODO: Adapt for OSX
		typedef int_fast8_t		Int8;
		typedef int_fast16_t	Int16;
		typedef int_fast32_t	Int32;
		typedef int_fast64_t	Int64;

		typedef uint_fast8_t	Uint8;
		typedef uint_fast16_t	Uint16;
		typedef uint_fast32_t	Uint32;
		typedef uint_fast64_t	Uint64;

#endif
}
