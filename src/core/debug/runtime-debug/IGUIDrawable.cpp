#include "IGUIDrawable.h"

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

#ifdef BOREALIS_WIN

//#include "../../graphics/helpers/helpers.h"
//#include "../../graphics/d3d12/borealis_d3d12.h"
//#include "imgui/imgui.h"


namespace Borealis::Runtime::Debug
{
	void IGUIDrawable::ToggleWindow()
	{
		isOpen = !isOpen;
	}

	bool IGUIDrawable::IsOpen()
	{
		return isOpen;
	}

	ImFont* IGUIDrawable::inter_light = nullptr;
	ImFont* IGUIDrawable::inter_bold = nullptr;
}

#endif

#endif
