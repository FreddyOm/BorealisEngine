#pragma once
#include "IGUI_drawable.h"
#include <string>

struct ImFont;

namespace Borealis::Runtime::Debug
{
	class EditorWindow : public IGUIDrawable
	{
	public:

		/// <summary>
		/// Constructor of the editor window.
		/// </summary>
		/// <param name="windowName">The name of the window.</param>
		EditorWindow(std::string winName) :
			windowName(winName), IGUIDrawable(true)
		{ }

		/// <summary>
		/// The destructor of the editor window.
		/// </summary>
		~EditorWindow()
		{ }

		void UpdateDrawable() override;

	protected:
		std::string windowName = "";

		// Inherited via IGUIDrawable
		void OnGui() override;

	};
}
