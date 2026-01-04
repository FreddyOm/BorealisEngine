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
		ImGui_ImplWin32_EnableDpiAwareness();
		float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

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

		// TODO: Hook the editors input calls to the engines input
		//Input::InputCallback = &ImGui_ImplWin32_WndProcHandler;

		// Setup scaling
		ImGuiStyle& style = ImGui::GetStyle();
		style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
		
		// TODO: Make sure the correct graphics backend is used

		Assert(m_Renderer.m_GraphicsBackend != GraphicsBackend::UNDEFINED,
			"Cannot initialize Dear Imgui for undefined graphics backend!");

		// Windwos only graphics APIs will always use Win32 in Borealis for now!
		if (m_Renderer.m_GraphicsBackend == GraphicsBackend::D3D11 || m_Renderer.m_GraphicsBackend == GraphicsBackend::D3D12)
		{
			Assert(ImGui_ImplWin32_Init(reinterpret_cast<void*>(m_Renderer.m_PipelineDesc.SwapChain.WindowHandle)),
				"Failed to initialize the runtime debugger GUI with Win32.");
		}
		
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
				d3d12InitInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
				d3d12InitInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
				d3d12InitInfo.SrvDescriptorHeap = pD3D12Renderer->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).Get();
				d3d12InitInfo.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) { return g_SRVDescHeapAllocator.Alloc(out_cpu_handle, out_gpu_handle); };
				d3d12InitInfo.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) { return g_SRVDescHeapAllocator.Free(cpu_handle, gpu_handle); };

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

	void RuntimeDebugger::UpdateDrawable()
	{
		Assert(initialized, "Cannot draw GUI when not initialized.");

		static Graphics::GraphicsBackend graphicsBackend = m_Renderer.m_GraphicsBackend;
		//static BorealisD3D12Renderer* pRenderer = dynamic_cast renderer;

		switch (graphicsBackend)
		{
		case D3D11:
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			break;
		case D3D12:
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			break;
		case VULKAN:
			Assert(false, "Not yet implemented!");
			break;
		default:
			Assert(false, "Unsupported graphics backend for runtime debugger GUI!");
			break;
		}

		ImGui::NewFrame();

		OnGui();

		ImGui::Render();
		p_drawData = ImGui::GetDrawData();

		switch (graphicsBackend)
		{
		case D3D11:
			p_drawData = ImGui::GetDrawData();
			ImGui_ImplDX11_RenderDrawData(p_drawData);
			break;
		case D3D12:
			// TODO: Push common rendering code to BorealisD3D12Renderer
			// TODO: Move ImGui specific code from BorealisD3D12Renderer to here
			
			/*static*/ BorealisD3D12Renderer* pD3D12Renderer = dynamic_cast<BorealisD3D12Renderer* const>(&m_Renderer);
			Assert(pD3D12Renderer != nullptr, "Failed to cast the renderer to BorealisD3D12Renderer.");
			static HRESULT hResult;
			hResult = S_OK;
			
			// Get back buffer index
			const UINT backBufferIdx = pD3D12Renderer->GetSwapChain()->GetCurrentBackBufferIndex();
			
			// Waiting for the last frame to finish
			FrameContext* frameCtx = pD3D12Renderer->WaitForNextFrameContext();
			
			// Reset command allocator
			hResult = frameCtx->CommandAllocator->Reset();
			Assert(hResult == S_OK, "Failed to reset command allocator in preparation for the new frame!");
			
			// Reset command list
			hResult = pD3D12Renderer->GetCommandList()->Reset(frameCtx->CommandAllocator.Get(), nullptr);
			Assert(hResult == S_OK, "Failed to reset command list in preparation for the new frame!");
			
			// TODO: Try to wrap resource barriers in a helper function in BorealisD3D12Renderer
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = pD3D12Renderer->GetCurrentRenderTarget();
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			
			/*hResult = pD3D12Renderer->GetCommandList()->Reset(frameCtx->CommandAllocator.Get(), nullptr);
			Assert(hResult == S_OK, StrFromHResult(hResult));*/
			
			pD3D12Renderer->GetCommandList()->ResourceBarrier(1, &barrier);
		    
			// Render Dear ImGui graphics | record commands
			static float clear_color_with_alpha[4] = { 0.2, 0.2, 0.2, 1 };
			pD3D12Renderer->GetCommandList()->ClearRenderTargetView(pD3D12Renderer->GetRTVDescriptor(backBufferIdx), clear_color_with_alpha, 0, nullptr);
			pD3D12Renderer->GetCommandList()->OMSetRenderTargets(1, &pD3D12Renderer->GetRTVDescriptor(backBufferIdx), FALSE, nullptr);
			pD3D12Renderer->GetCommandList()->SetDescriptorHeaps(1, pD3D12Renderer->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV).GetAddressOf());
			
			// Draw render data
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pD3D12Renderer->GetCommandList());
			
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			pD3D12Renderer->GetCommandList()->ResourceBarrier(1, &barrier);
			
			// Close and execute the command list
			//ID3D12CommandList* const pCommandList = pD3D12Renderer->m_CommandList.GetAddressOf();

			// Issue is probably pCommandList !!
			hResult = pD3D12Renderer->GetCommandList()->Close();
			Assert(hResult == S_OK, StrFromHResult(hResult));

			// Execute the command list
			pD3D12Renderer->GetCommandQueue()->ExecuteCommandLists(1, (ID3D12CommandList* const*) pD3D12Renderer->m_CommandList.GetAddressOf());
			hResult = pD3D12Renderer->GetCommandQueue()->Signal(pD3D12Renderer->m_CommandQueueFence.Get(), ++pD3D12Renderer->m_LastSignaledFenceValue);
			Assert(hResult == S_OK, StrFromHResult(hResult));

			frameCtx->FenceValue = pD3D12Renderer->m_LastSignaledFenceValue;

			hResult = pD3D12Renderer->PresentFrame();
			Assert(hResult == S_OK, StrFromHResult(hResult));
			
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

			ImGui::ShowDemoWindow();

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