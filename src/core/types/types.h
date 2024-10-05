#pragma once
#include "../../borealis_engine.h"
#include <cstdint>

namespace Borealis
{	
	namespace Types
	{
#ifdef BOREALIS_WIN

		typedef int_fast8_t		Int8;
		typedef int_fast16_t	Int16;
		typedef int_fast32_t	Int32;
		typedef int_fast64_t	Int64;
		
		typedef uint_fast8_t	Uint8;
		typedef uint_fast16_t	Uint16;
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

#endif;

#ifndef NDEBUG

		static_assert(sizeof(Int8) == 1, "Type size of Int8 is not correct on the current platform configuration!");
		static_assert(sizeof(Int16) == 2, "Type size of Int16 is not correct on the current platform configuration!");
		static_assert(sizeof(Int32) == 4, "Type size of Int32 is not correct on the current platform configuration!");
		static_assert(sizeof(Int64) == 8, "Type size of Int64 is not correct on the current platform configuration!");
		
		static_assert(sizeof(Uint8) == 1, "Type size of Uint8 is not correct on the current platform configuration!");
		static_assert(sizeof(Uint16) == 2, "Type size of Uint16 is not correct on the current platform configuration!");
		static_assert(sizeof(Uint32) == 4, "Type size of Uint32 is not correct on the current platform configuration!");
		static_assert(sizeof(Uint64) == 8, "Type size of Uint64 is not correct on the current platform configuration!");

#endif

	}
}
