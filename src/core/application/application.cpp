#include "application.h"
#include "../graphics/graphics.h"
#ifdef BOREALIS_WIN
#include "../graphics/d3d12/borealis_d3d12.h"
#endif

using namespace Borealis::Core;
using namespace Borealis::Input;
using namespace Borealis::Memory;
using namespace Borealis::Graphics;
using namespace Borealis::Runtime::Debug;
using namespace Borealis::Graphics::Helpers;

namespace Borealis::Core
{
	Application::Application()
	{
		// Init all locators
		WindowLocator::Initialize();
		RendererLocator::Initialize();
		InputSystemLocator::Initialize();

		InitializeApp();
	}

	Application::Application(const char* appName)
	{
		WindowLocator::Initialize();
		RendererLocator::Initialize();
		InputSystemLocator::Initialize();

		InitializeApp();
	}


	Application::~Application()
	{
		DeinitializeApp();
	}

	bool Application::IsRunning() const
	{
		return m_Window->IsOpen();
	}

	void Application::Update()
	{
		while (m_Window->IsOpen())
		{
			m_InputSystem->UpdateInputState();
			m_Window->UpdateWindow();

#ifdef WIN32
#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)

			m_RuntimeDebugger->UpdateDrawable();
#endif		
#endif
		}
	}


	void Application::InitializeApp(const char* appName)
	{

		Memory::MemAllocJanitor janitor(Memory::MemAllocatorContext::CORESYS);
		// Init all relevant systems
		Log("Initializing app \"%s\"", appName);


		Log("Initializing Window...");
		m_Window = RefCntAutoPtr<Window>::Allocate(appName);
		WindowLocator::Provide(RefCntAutoPtr<Window>::DynamicCastTo<BorealisWindow>(m_Window));
		Assert(m_Window.IsValid(), "Failed to create Window!");
				
		m_Window->OpenWindow();

		// TODO: Investigate if swapchain gets resized automatically! 
		// Currently it seems as though the back buffer size is always correct.
		// Which is probably not regulated by BorealisEngine but rather by GLFW, but how? 
		// And do I need PipelineDesc::SwapChain anymore?

#ifdef BOREALIS_WIN
		Log("Creating Rendering Pipeline ...");
		PipelineDesc desc{};
		desc.SwapChain.WindowHandle = m_Window->GetNativeWindowHandle();
		desc.SwapChain.BufferHeight = m_Window->GetWindowHeight();
		desc.SwapChain.BufferWidth = m_Window->GetWindowWidth();
		
		Log("Initializing Renderer ...");
		RefCntAutoPtr<BorealisD3D12Renderer> renderer = RefCntAutoPtr<BorealisD3D12Renderer>::Allocate(desc);
		m_Renderer = RefCntAutoPtr<BorealisD3D12Renderer>::DynamicCastTo<Helpers::IBorealisRenderer>(renderer);
		RendererLocator::Provide(m_Renderer);
		Assert(m_Renderer.IsValid(), "Failed to create Renderer!");
		InitD3D12LiveObjects();
#endif

		Log("Initializing Input System ...");
		m_InputSystem = RefCntAutoPtr<InputSystem>::Allocate(m_Window->GetGLFWWindow());
		InputSystemLocator::Provide(RefCntAutoPtr<InputSystem>::DynamicCastTo<IInputSystemBase>(m_InputSystem));
		Assert(m_InputSystem.IsValid(), "Failed to create Input System!");

		m_Renderer->InitializePipeline();

#ifdef BOREALIS_WIN
		Log("Initializing Runtime Debugger ...");
		m_RuntimeDebugger = RefCntAutoPtr<RuntimeDebugger>::Allocate();
		m_RuntimeDebugger->Attatch();
#endif
		//  attatch runtime debugger
	}
	
	void Application::DeinitializeApp()
	{
#ifdef BOREALIS_WIN
		m_RuntimeDebugger->Detatch();
#endif
		m_Renderer->DeinitializePipeline();

		// Manually reset global data in order to avoid issues with ReportLiveObjects
		RendererLocator::Reset();
		WindowLocator::Reset();
		InputSystemLocator::Reset();
	}
}