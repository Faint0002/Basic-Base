#pragma once
#include "Includes.hpp"
#include "VMT.hpp"

namespace Base::Render {
	class Render {
	public:
		Render();
		~Render();
	public:
		void Start();
		void Stop();
	public:
		static LRESULT WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
		WNDPROC m_OriginialWndProc;
	public:
		static constexpr auto VMTNumberOfFuncs = 19;
		static constexpr auto PresentIndex = 8;
		static HRESULT Present(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
		static constexpr auto ResizeBuffersIndex = 13;
		static HRESULT ResizeBuffers(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);
		void OnPresent();
	public:
		ID3D11Device* m_Device;
		ID3D11DeviceContext* m_Context;
		IDXGISwapChain* m_Swapchain;
		ID3D11RenderTargetView* m_TargetView;
		DXGI_SWAP_CHAIN_DESC m_Desc;
		Core::Mem::VMT m_VMT;
	};
}
inline std::unique_ptr<Base::Render::Render> g_Render;