#pragma once
#include "../../config.h"
#include "../types/types.h"
#include "../memory/ref_cnt_auto_ptr.h"

namespace Borealis::IO
{
	BOREALIS_API Memory::RefCntAutoPtr<char> ReadFile(const char* filePath, Memory::MemAllocatorContext allocCntxt, Types::uint64& out_dataSize, const char* readMode = "rb");


}