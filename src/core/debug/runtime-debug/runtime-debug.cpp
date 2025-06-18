#include "runtime-debug.h"

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

#include "../logger.h"

#ifdef BOREALIS_WIN

#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_dx12.h"

//#include "../../graphics/d3d11/borealis_d3d11.h"
//#include "../../graphics/d3d12/borealis_d3d12.h"
//#include "imgui/imgui_impl_vulkan.h"

#include "../../graphics/helpers/helpers.h"
#else

//#include "imgui/imgui_impl_vulkan.h"

#endif

using namespace ImGui;
using namespace Borealis::Graphics::Helpers;
using namespace Borealis::Graphics;

namespace Borealis::Runtime::Debug
{
	/// <summary>
/// Set new GUI frame.
/// </summary>
	void IGUIDrawable::PreGUIUpdate()
	{
		if (initialized)
		{
			ImGui_ImplDX11_NewFrame();

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
		}
	}

	/// <summary>
	/// Render the GUI.
	/// </summary>
	void IGUIDrawable::PostGUIUpdate()
	{
		if (initialized)
		{
			ImGui::Render();
			p_drawData = ImGui::GetDrawData();
			ImGui_ImplDX11_RenderDrawData(p_drawData);
		}
	}

	/// <summary>
	/// Returns a list of pointers to drawable gui elements.
	/// </summary>
	/// <returns>A vector of gui drawables.</returns>
	std::vector<IGUIDrawable*>* IGUIDrawable::GetGUIDrawablePtrs()
	{
		return &guiDrawables;
	}

	void IGUIDrawable::ToggleWindow()
	{
		isOpen = !isOpen;
	}

	bool IGUIDrawable::IsOpen()
	{
		return isOpen;
	}


	/// <summary>
	/// Uninitializes the gui context.
	/// </summary>
	void IGUIDrawable::UnInitializeGUI() const
	{
		if (initialized)
		{
			initialized = false;

			ImGui_ImplDX12_Shutdown();
			ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();

			guiDrawables.clear();
			guiDrawables.~vector();
		}
	}

	// @TODO: Fix runtime error in release config where font atlas is nullptr

	/// <summary>
	/// Initializes the gui context.
	/// </summary>
	void IGUIDrawable::InitializeGUI()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		//lexend_light = io.Fonts->AddFontFromFileTTF("./resources/fonts/Lexend-Light.ttf", 14);
		inter_light = io.Fonts->AddFontFromFileTTF("./resources/fonts/Inter-Light.ttf", 14);

		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 14.0f; // Use if you want to make the icon monospaced
		static const ImWchar icon_ranges[] = { 0xf000, 0xf372, 0 };
		io.Fonts->AddFontFromFileTTF("./resources/fonts/IconFont.ttf", 14.0f, &config, icon_ranges);

		config.MergeMode = false;
		inter_bold = io.Fonts->AddFontFromFileTTF("./resources/fonts/Inter-Bold.ttf", 11);

		config.MergeMode = true;
		io.Fonts->AddFontFromFileTTF("./resources/fonts/IconFont.ttf", 14.0f, &config, icon_ranges);

		io.Fonts->Build();

		// Use CCE Colors
		ImGui::StyleColorsBorealis();

		// Set Window specs1
		ImGui::GetStyle().WindowRounding = 4.0f;
		ImGui::GetStyle().ChildRounding = 4.0f;
		ImGui::GetStyle().FrameRounding = 1.0f;
		ImGui::GetStyle().GrabRounding = 0.0f;
		ImGui::GetStyle().PopupRounding = 4.0f;
		ImGui::GetStyle().ScrollbarRounding = 4.0f;

		// Hook the editors input calls to the engines input
		//Input::InputCallback = &ImGui_ImplWin32_WndProcHandler;

		// TODO: Make sure the correct graphics backend is used

		//Assert(ImGui_ImplWin32_Init(GetActiveWindow()), "Failed to initialize the runtime debugger GUI with Win32.");
		// For now, fall-through because windows impl is always reliant on win32
		

		//IBorealisRenderer* renderer = (IBorealisRenderer*) new BorealisD3D12Renderer();	// TODO: Temp solution or fixing compiler errors

		//switch (renderer->GraphicsBackend)
		//{
		//	case GraphicsBackend::D3D11:
		//	{
		//		/*Assert(ImGui_ImplDX11_Init(Graphics::g_pDevice.Get(), Graphics::g_pContext.Get()),
		//			"Failed to initialize the runtime debugger GUI with D3D11.");*/
		//		break;
		//	}
		//	case GraphicsBackend::D3D12:
		//	{
		//		BorealisD3D12Renderer* const pD3D12Renderer = dynamic_cast<BorealisD3D12Renderer* const>(renderer);
		//		Assert(pD3D12Renderer != nullptr, "Failed to cast generic IBorealisRenderer to BorealisD3D12Renderer renderer!");

		//		Assert(ImGui_ImplWin32_Init(pD3D12Renderer->GetPipelineDesc().SwapChain.WindowHandle), "Failed to initialize the runtime debugger GUI with Win32.");

		//		ImGui_ImplDX12_InitInfo d3d12InitInfo{};
		//		d3d12InitInfo.Device = pD3D12Renderer->GetDevice();
		//		d3d12InitInfo.CommandQueue = pD3D12Renderer->GetCommandQueue();
		//		d3d12InitInfo.NumFramesInFlight = 0;	// TODO: Fix me!
		//		//d3d12InitInfo.SrvDescriptorAllocFn = ;
		//		//d3d12InitInfo.SrvDescriptorFreeFn = ;
		//		//d3d12InitInfo.SrvDescriptorHeap = ;

		//		Assert(ImGui_ImplDX12_Init(&d3d12InitInfo), "Failed to initialize the runtime debugger GUI with D3D12.");
		//		break;
		//	}
		//	case Graphics::GraphicsBackend::VULKAN:
		//	{
		//		//Assert(ImGui_ImplSDL2_InitForVulkan(), "Failed to initialize the runtime debugger GUI with Vulkan.");
		//		// break;
		//	}

		//	default:
		//	{
		//		Assert(false, "Couldn't resolve the rendering backend!");
		//		break;
		//	}

		//}

	}

	/// <summary>
	/// A flag that indicates wether or not the gui is initialized.
	/// </summary>
	bool IGUIDrawable::initialized = false;

	ImFont* IGUIDrawable::inter_light = nullptr;
	ImFont* IGUIDrawable::inter_bold = nullptr;

	/// <summary>
	/// A list of registered gui drawables.
	/// </summary>
	std::vector<IGUIDrawable*> IGUIDrawable::guiDrawables = {};

	/// <summary>
	/// ImGuis draw data. 
	/// </summary>
	ImDrawData* IGUIDrawable::p_drawData = nullptr;
}

#endif