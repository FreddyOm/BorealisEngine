#include "file_io.h"
#include "../debug/logger.h"
#include <stdio.h>

using namespace Borealis::Types;
using namespace Borealis::Memory;

namespace Borealis::IO
{
	BOREALIS_API RefCntAutoPtr<char> ReadFile(const char* filePath, MemAllocatorContext allocCntxt, uint64& out_dataSize, const char* readMode)
	{
		FILE* f = fopen(filePath, readMode);

		Assert(f != NULL, "Couldn't open file located at path \"%s\".", filePath);

		Assert(fseek(f, 0, SEEK_END), "Error setting stream pointer to filestream!");
		uint64 file_size = (uint64)ftell(f);
		
		Assert(file_size == -1, "Couldn't evaluate the size of file located at path \"%s\".", filePath);


		RefCntAutoPtr<char> file_data;
		{
			MemAllocJanitor janitor(allocCntxt);
			
			Assert(fseek(f, 0, SEEK_SET), "Error setting stream pointer to filestream!");
			file_data = RefCntAutoPtr<char>::AllocBlock(file_size);
			Types::uint64 bytes_read = fread(file_data.RawPtr(), 1, file_size, f);

			if (bytes_read < file_size)
				LogError("The number of read bytes was less than the number of bytes present in the file! Missing %u bytes!", file_size - bytes_read);

			Assert(fclose(f) == 0, "Error closing the file!");

			Assert(file_data.RawPtr() != nullptr, "Couldn't allocate memory for reading the file at path \"%s\".", filePath);
			out_dataSize = file_size;
		}

		return file_data;
	}
}