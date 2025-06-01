#pragma once 

#if defined(BOREALIS_WIN)

namespace Borealis::Graphics::Helpers
{
	#define BOREALIS_IID_PPV_ARGS(ppType) __uuidof(**(ppType)), (void**)ppType

	

}

#endif