#pragma once
#include "../../helpers/macros.h"
//#include "../../../config.h"
//#include "../../graphics/pipeline_config.h"

// TODO: Figure out how I can setup and use debug gui only in debug and relwithdebinfo builds
//#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

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

		virtual ~IGUIDrawable()
		{

		}

		BOREALIS_DELETE_COPY_CONSTRUCT(IGUIDrawable)
		BOREALIS_DELETE_MOVE_CONSTRUCT(IGUIDrawable)
		BOREALIS_DELETE_COPY_ASSIGN(IGUIDrawable)
		BOREALIS_DELETE_MOVE_ASSIGN(IGUIDrawable)

		/// <summary>
		/// Method called when updating the editor window.
		/// </summary>
		virtual void OnGui() = 0;

		/// <summary>
		/// Method that specifies how the OnGui is called.
		/// </summary>
		virtual void Update() = 0;

		void ToggleWindow();
		bool IsOpen();
		
		static ImFont* inter_light;
		static ImFont* inter_bold;
		static ImFont* lexend_light;
		static ImFont* lexend_bold;
		static ImFont* calibri;
		static ImFont* calibri_light;
		static ImFont* calibri_bold;

	protected:

		bool isOpen = false;
	};
}
//#endif