#include "Render.hpp"
#include "Logger.hpp"
#include "Gui.hpp"

namespace Base::Render {
	//Get our swapchain, and get our VMT hook plus set values for our swapchain and our VMT hook.
	Render::Render() : m_Swapchain(*g_Pointers->m_GetSwapchainDevice), m_VMT(*g_Pointers->m_GetSwapchainDevice, VMTNumberOfFuncs) {
		logEntry(LogColor::LightBlue, "DirectX", "Allocating D3D11 Info...");
		if (SUCCEEDED(m_Swapchain->GetDevice(__uuidof(ID3D11Device), (void**)&m_Device))) //Get our swapchain device and set it to our dummy device
			logEntry(LogColor::LightBlue, "DirectX", "Attached the swapchain device!");
		else
			logEntry(LogColor::Intensify, "DirectX", "Failed to allocate D3D11 Info!");
		m_Swapchain->GetDesc(&m_Desc);
		m_Device->GetImmediateContext(&m_Context);
		logEntry(LogColor::LightBlue, "DirectX", "Initializing ImGui context and backends...");
		ImGui::CreateContext();
		//Init ImGui backends (DX11, and Win32)
		ImGui_ImplDX11_Init(m_Device, m_Context);
		ImGui_ImplWin32_Init(m_Desc.OutputWindow);
		//Add our colors, fonts etc.
		g_Gui->GuiInit();
		logEntry(LogColor::LightBlue, "DirectX", "Initialized ImGui context and backends!");
		Start();
		logEntry(LogColor::LightBlue, "DirectX", "Allocated D3D11 Info!");
	}
	Render::~Render() {
		Stop();
		ImGui::DestroyContext();
	}
	void Render::Start() {
		logEntry(LogColor::LightBlue, "DirectX", "Registering Window Proc...");
		m_OriginialWndProc = (WNDPROC)SetWindowLongPtr(g_Pointers->m_Hwnd, GWLP_WNDPROC, (LONG_PTR)WindowProc);
		m_VMT.Hook(&Present, PresentIndex);
		m_VMT.Hook(&ResizeBuffers, ResizeBuffersIndex);
		m_VMT.Enable();
	}
	void Render::Stop() {
		SetWindowLongPtr(g_Pointers->m_Hwnd, GWLP_WNDPROC, (LONG_PTR)m_OriginialWndProc);
		m_VMT.Disable();
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplDX11_Shutdown();
	}
	void Render::OnPresent() {
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::GetIO().MouseDrawCursor = g_Gui->m_Opened;
		if (g_Gui->m_Opened)
			g_Gui->GuiWindow();
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	LRESULT Render::WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
		if (Msg == WM_KEYUP && wParam == VK_INSERT) {
			g_Gui->m_Opened = !g_Gui->m_Opened;
		}
		if (g_Gui->m_Opened)
			ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);
		return CallWindowProcW(g_Render->m_OriginialWndProc, hWnd, Msg, wParam, lParam);
	}
	HRESULT Render::Present(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
		g_Render->OnPresent();
		return g_Render->m_VMT.GetOriginal<decltype(&Present)>(PresentIndex)(swapChain, syncInterval, flags);
	}
	HRESULT Render::ResizeBuffers(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
		ImGui_ImplDX11_InvalidateDeviceObjects(); //Pre Resize
		auto result = g_Render->m_VMT.GetOriginal<decltype(&ResizeBuffers)>(ResizeBuffersIndex)(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
		if (SUCCEEDED(result)) {
			ImGui_ImplDX11_CreateDeviceObjects(); //Post Resize
			return result;
		}
		return g_Render->m_VMT.GetOriginal<decltype(&ResizeBuffers)>(ResizeBuffersIndex)(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
	}
}