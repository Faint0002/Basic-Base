#include "Cheat.hpp"

namespace Base::Core {
	void MainEntry::Initialize() {
		g_Logger = std::make_unique<Window::Log>();
		MH_Initialize();
		g_Logger->Initialize();
		g_Pointers = std::make_unique<Mem::Pointers>();
		g_Pointers->ScanPatterns();
		g_nativeInvoker.cacheHandlers();
		g_Gui = std::make_unique<DirectX::Gui>();
		g_Render = std::make_unique<Render::Render>();
		g_fbrMgr.add(std::make_shared<fiber>(DirectX::Gui::GuiLoop), "guiFbr");
		g_fbrPool.registerFbrPool();
		g_Hooking.Enable();
	}
	void MainEntry::Uninitialize() {
		g_Hooking.Disable();
		MH_Uninitialize();
		g_fbrMgr.removeAll();
		g_Logger->Shutdown();
		g_Render.reset();
		g_Gui.reset();
		g_Pointers.reset();
	}
}