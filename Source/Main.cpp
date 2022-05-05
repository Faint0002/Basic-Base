#include "Includes.hpp"
#include "Cheat.hpp"
using namespace Base::Core;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        g_hmModule = hModule;
        g_hModuleHandle = CreateThread(nullptr, NULL, [](LPVOID mod) -> DWORD {
            g_MainEntry = std::make_unique<MainEntry>();
            g_MainEntry->Initialize();
            logEntry(LogColor::White, "Init", "Finished loading! Welcome to Basic Base.");
			while (g_Running) {
				if (GetAsyncKeyState(VK_END)) {
                    g_Running = false;
                }
                std::this_thread::sleep_for(10ms);
			}
            g_MainEntry->Uninitialize();
            logEntry(LogColor::White, "Uninit", "Finished unloading.");
            g_Logger.reset();
            g_MainEntry.reset();
            FreeLibraryAndExitThread(g_hmModule, g_dwMainThreadID);
            return 0;
        }, hModule, g_dwMainThreadID, nullptr);
    }
    return TRUE;
}