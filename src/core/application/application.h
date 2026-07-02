#pragma once
#include "../../config.h"
#include "../helpers/macros.h"
#include "../graphics/graphics.h"
#include "../window/window.h"
#include "../input/input.h"

namespace Borealis::Core
{
	class BOREALIS_API Application
	{
	public: 

		Application();
		Application(const char* appName);

		~Application();

		BOREALIS_DELETE_COPY_CONSTRUCT(Application)
		BOREALIS_DELETE_MOVE_CONSTRUCT(Application)
		BOREALIS_DELETE_COPY_ASSIGN(Application)
		BOREALIS_DELETE_MOVE_ASSIGN(Application)


		bool IsRunning() const;
		void Update();

	private:

		void InitializeApp(const char* appName = "Borealis Engine");
		void DeinitializeApp();

	private:

		// TODO: What of this should be a RefCntAutoPtr?
		Memory::RefCntAutoPtr<Graphics::BorealisD3D12Renderer> m_Renderer;
		Memory::RefCntAutoPtr<Core::Window> m_Window;
		Memory::RefCntAutoPtr<Input::InputSystem> m_InputSystem;
	};
}