#pragma once

#include "IGUI_drawable.h"
#include "../../helpers/macros.h"

namespace Borealis::Runtime::Debug
{
	class MemoryDebugger : public IGUIDrawable
	{
		MemoryDebugger()
		{}

		~MemoryDebugger() = default;

		BOREALIS_DEFAULT_COPY_CONSTRUCT(MemoryDebugger)
		BOREALIS_DEFAULT_MOVE_CONSTRUCT(MemoryDebugger)
		BOREALIS_DEFAULT_COPY_ASSIGN(MemoryDebugger)
		BOREALIS_DEFAULT_MOVE_ASSIGN(MemoryDebugger)
	};
}