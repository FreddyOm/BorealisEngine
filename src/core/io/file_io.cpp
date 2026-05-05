#include "file_io.h"
#include "../debug/logger.h"

using namespace Borealis::Types;
using namespace Borealis::Memory;

namespace Borealis::IO
{
	BOREALIS_API RefCntAutoPtr<char> ReadFile(const char* filePath, MemAllocatorContext allocCntxt, uint64& out_dataSize, const char* readMode)
	{
		FILE* f = fopen(filePath, readMode);

		Assert(f != NULL, "Couldn't open file located at path \"%s\".", filePath);

		fseek(f, 0, SEEK_END);
		uint64 file_size = (uint64)ftell(f);
		
		Assert(file_size == -1, "Couldn't evaluate the size of file located at path \"%s\".", filePath);


		RefCntAutoPtr<char> file_data;
		{
			MemAllocJanitor janitor(allocCntxt);
			
			fseek(f, 0, SEEK_SET);
			file_data = RefCntAutoPtr<char>::AllocBlock(file_size);
			fread(file_data.RawPtr(), 1, file_size, f);
			fclose(f);

			Assert(file_data.RawPtr() != nullptr, "Couldn't allocate memory for reading the file at path \"%s\".", filePath);
			out_dataSize = file_size;
		}

		return file_data;
	}
}