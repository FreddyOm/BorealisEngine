#pragma once
#include "../../../config.h"

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

#include "imgui/imgui.h"
#include <vector>

namespace Borealis::Runtime::Debug
{
	struct IGUIDrawable
	{
	public:
		IGUIDrawable(bool isOpen = false)
			: isOpen(isOpen)
		{
			if (!initialized)
			{
				initialized = true;
				InitializeGUI();
			}

			guiDrawables.emplace_back(this);
		}

		~IGUIDrawable()
		{
			UnInitializeGUI();
		}

		/// <summary>
		/// Method called when updating the editor window.
		/// </summary>
		virtual void OnGui() = 0;

		/// <summary>
		/// Method that specifies how the OnGui is called.
		/// </summary>
		virtual void UpdateDrawable(ImFont* font = inter_bold) = 0;

		static void PreGUIUpdate();
		static void PostGUIUpdate();
		static std::vector<IGUIDrawable*>* GetGUIDrawablePtrs();

		void ToggleWindow();
		bool IsOpen();

	private:
		void UnInitializeGUI() const;
		void InitializeGUI();

	protected:
		bool isOpen = false;
		static bool initialized;

		static ImFont* inter_light;
		static ImFont* inter_bold;

	private:
		static std::vector<IGUIDrawable*> guiDrawables;
		static ImDrawData* p_drawData;
	};
}

#endif