#pragma once
#include "../../../config.h"
#include "../../graphics/pipeline_config.h"

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

struct ImFont;

namespace Borealis::Runtime::Debug
{
	struct IGUIDrawable
	{
	public:
		IGUIDrawable(bool isOpen = false)
			: isOpen(isOpen)
		{
			//guiDrawables.emplace_back(this);
		}

		~IGUIDrawable()
		{

		}

		/// <summary>
		/// Method called when updating the editor window.
		/// </summary>
		virtual void OnGui() = 0;

		/// <summary>
		/// Method that specifies how the OnGui is called.
		/// </summary>
		virtual void UpdateDrawable() = 0;

		void ToggleWindow();
		bool IsOpen();
		
		static ImFont* inter_light;
		static ImFont* inter_bold;

	protected:
		bool isOpen = false;

	};
}
#endif