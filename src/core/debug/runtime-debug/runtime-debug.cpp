#include "runtime-debug.h"

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

#include "../logger.h"
#include "imgui/imgui.h"

#ifdef BOREALIS_WIN

#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_dx12.h"

//#include "../../graphics/d3d11/borealis_d3d11.h"
#include "../../graphics/d3d12/borealis_d3d12.h"
//#include "imgui/imgui_impl_vulkan.h"

#include "../../graphics/helpers/helpers.h"
#include "../../graphics/pipeline_config.h"
#else

//#include "imgui/imgui_impl_vulkan.h"

#endif

using namespace ImGui;
using namespace Borealis::Graphics::Helpers;
using namespace Borealis::Graphics;

namespace Borealis::Runtime::Debug
{
	/// <summary>
	/// Uninitializes the gui context.
	/// </summary>
	void RuntimeDebugger::UninitializeGUI()
	{
		if (initialized)
		{
			initialized = false;

			ImGui_ImplDX12_Shutdown();
			//ImGui_ImplDX11_Shutdown();
			ImGui_ImplWin32_Shutdown();

			//guiDrawables.clear();
		}
	}

	// TODO: Fix runtime error in release config where font atlas is nullptr

	/// <summary>
	/// Initializes the gui context.
	/// </summary>
	void RuntimeDebugger::InitializeGUI()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		//lexend_light = io.Fonts->AddFontFromFileTTF("./resources/fonts/Lexend-Light.ttf", 14);
		
		inter_bold = io.Fonts->AddFontFromFileTTF("D:/02_Repositories/BorealisEngine/out/build/x64-Debug/sandbox/resources/fonts/Inter-Bold.ttf", 14.0f);
		inter_light = io.Fonts->AddFontFromFileTTF("D:/02_Repositories/BorealisEngine/out/build/x64-Debug/sandbox/resources/fonts/Inter-Light.ttf", 14.0f);

		ImFontConfig config;
		config.MergeMode = true;
		config.GlyphMinAdvanceX = 14.0f; // Use if you want to make the icon monospaced
		static const ImWchar icon_ranges[] = { 0xf000, 0xf372, 0 };
		io.Fonts->AddFontFromFileTTF("D:/02_Repositories/BorealisEngine/out/build/x64-Debug/sandbox/resources/fonts/IconFont.ttf", 14.0f, &config, icon_ranges);

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

		Assert(m_Renderer.m_GraphicsBackend != GraphicsBackend::UNDEFINED,
			"Cannot initialize Dear Imgui for undefined graphics backend!");

		Assert(ImGui_ImplWin32_Init(reinterpret_cast<void*>(m_Renderer.m_PipelineDesc.SwapChain.WindowHandle)), 
			"Failed to initialize the runtime debugger GUI with Win32.");
		// For now, fall-through because windows impl is always reliant on win32
		
		switch (m_Renderer.m_GraphicsBackend)
		{
			case GraphicsBackend::D3D11:
			{
				/*Assert(ImGui_ImplDX11_Init(Graphics::g_pDevice.Get(), Graphics::g_pContext.Get()),
					"Failed to initialize the runtime debugger GUI with D3D11.");*/
				break;
			}
			case GraphicsBackend::D3D12:
			{
				BorealisD3D12Renderer* const pD3D12Renderer = dynamic_cast<BorealisD3D12Renderer* const>(&m_Renderer);
				Assert(pD3D12Renderer != nullptr, "Failed to cast generic IBorealisRenderer to BorealisD3D12Renderer renderer!");

				ImGui_ImplDX12_InitInfo d3d12InitInfo{};
				d3d12InitInfo.Device = pD3D12Renderer->GetDevice();
				d3d12InitInfo.CommandQueue = pD3D12Renderer->GetCommandQueue();
				d3d12InitInfo.NumFramesInFlight = 2;	// TODO: Fix me!
				//d3d12InitInfo.SrvDescriptorAllocFn = ;
				//d3d12InitInfo.SrvDescriptorFreeFn = ;
				//d3d12InitInfo.SrvDescriptorHeap = ;

				Assert(ImGui_ImplDX12_Init(&d3d12InitInfo), "Failed to initialize the runtime debugger GUI with D3D12.");
				break;
			}
			case GraphicsBackend::VULKAN:
			{
				//Assert(ImGui_ImplSDL2_InitForVulkan(), "Failed to initialize the runtime debugger GUI with Vulkan.");
				// break;
			}

			case GraphicsBackend::UNDEFINED:
			default:
			{
				Assert(false, "Couldn't resolve the rendering backend!");
				break;
			}

		}

		initialized = true;
	}

	/// <summary>
	/// A flag that indicates wether or not the gui is initialized.
	/// </summary>
	//bool IGUIDrawable::initialized = false;


	/// <summary>
	/// A list of registered gui drawables.
	/// </summary>
	//std::vector<IGUIDrawable*> IGUIDrawable::guiDrawables = {};

	/// <summary>
	/// ImGuis draw data. 
	/// </summary>
	ImDrawData* RuntimeDebugger::p_drawData = nullptr;


	void RuntimeDebugger::UpdateDrawable(ImFont* font)
	{
		Assert(initialized, "Cannot draw GUI when not initialized.");

		static Graphics::GraphicsBackend graphicsBackend = m_Renderer.m_GraphicsBackend;
		//static BorealisD3D12Renderer* pRenderer = dynamic_cast renderer;

		switch (graphicsBackend)
		{
		case D3D11:
			ImGui_ImplDX11_NewFrame();
			break;
		case D3D12:
			ImGui_ImplDX12_NewFrame();
			break;
		default:
			ImGui_ImplWin32_NewFrame();
			break;
		}

		ImGui::NewFrame();
		

		OnGui();


		ImGui::Render();
		p_drawData = ImGui::GetDrawData();

		switch (graphicsBackend)
		{
		case D3D11:
			ImGui_ImplDX11_RenderDrawData(p_drawData);
			break;
		case D3D12:
			//ImGui_ImplDX12_RenderDrawData(p_drawData, );
			break;
		}
	}

	void RuntimeDebugger::OnGui()
	{
		if (isOpen)
		{
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			DrawCategoryButtons();
			DrawDebugInfoLabels();

			ImGui::PopStyleColor(3);
		}
	}

	void RuntimeDebugger::DrawCategoryButtons()
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(80, 200));

		ImGui::Begin("Debug Categories", &isOpen, flags);

		//for (Types::uint16 i = 0; i < categoryButtons.size(); ++i)
		//{
		//	categoryButtons[i].Draw(i + 1); // draw all added debug windows except this (which is at index 0)
		//}

		ImGui::End();
	}

	void RuntimeDebugger::DrawDebugInfoLabels()
	{
		ImGui::SetNextWindowPos(ImVec2(50, 0));
		ImGui::SetNextWindowSize(ImVec2(600, 40));

		ImGui::Begin("FPSStats", &isOpen, flags);

		/*for (Types::uint16 i = 0; i < debugLabels.size(); ++i)
		{
			debugLabels[i]->Draw();
		}*/

		ImGui::End();
	}

}

#endif