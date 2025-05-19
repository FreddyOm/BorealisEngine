#include "ref_cnt_auto_ptr.h"
#include <string>

namespace Borealis::Memory
{
	PoolAllocator RefCntInfoAllocator = PoolAllocator(4096, sizeof(RefCntAutoPtrInfo));

#ifdef NDEBUG

	std::string GetPointerInvalidReason(const REF_CNT_AUTO_PTR_STATE _state)
	{
		switch (_state)
		{
		case REF_CNT_AUTO_PTR_STATE::VALID:
		{
			Debug::LogError("The ref counted auto ptr was falsely claimed to be invalid!");
			return "The ref counted auto ptr was falsely claimed to be invalid!";
			break;
		}
		case REF_CNT_AUTO_PTR_STATE::INVALID_MEM_FREE:
		{
			return "The pointer was invalidated because the memory its data was attributed to was released by the respective allocator.";
			break;
		}
		case REF_CNT_AUTO_PTR_STATE::INVALID_MOVED:
		{
			return "The pointer was invalidated because it was moved to another reference instance.";
		}
		default:
			return "The pointer was invalidated.";
			break;
		}
		return std::string("Invalid ref counted auto pointer state!");
	}

#endif

}