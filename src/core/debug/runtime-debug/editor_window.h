#pragma once
#include "IGUI_drawable.h"
#include "../../helpers/macros.h"
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

		BOREALIS_DELETE_COPY_CONSTRUCT(EditorWindow)
		BOREALIS_DELETE_MOVE_CONSTRUCT(EditorWindow)
		BOREALIS_DELETE_COPY_ASSIGN(EditorWindow)
		BOREALIS_DELETE_MOVE_ASSIGN(EditorWindow)

		void UpdateDrawable() override;

	protected:
		std::string windowName = "";

		// Inherited via IGUIDrawable
		void OnGui() override;

	};
}
