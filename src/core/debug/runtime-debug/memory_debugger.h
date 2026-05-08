#pragma once

#include "IGUI_drawable.h"

namespace Borealis::Runtime::Debug
{
	class MemoryDebugger : public IGUIDrawable
	{
		MemoryDebugger()
		{}

		~MemoryDebugger() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(MemoryDebugger)
		BOREALIS_DELETE_MOVE_CONSTRUCT(MemoryDebugger)
		BOREALIS_DELETE_COPY_ASSIGN(MemoryDebugger)
		BOREALIS_DELETE_MOVE_ASSIGN(MemoryDebugger)


	};
}