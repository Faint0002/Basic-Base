#include "Pointers.hpp"
#include "Render.hpp"

namespace Base::Core::Mem {
	void Pointers::ScanPatterns() {
		//Run Script Threads (Used for the main hook) (Hooking\Main)
		g_mainBatch.add("RST", "45 33 F6 8B E9 85 C9 B8", [=](memory ptr) {
			m_ScriptThreads = ptr.sub(4).rip().sub(8).as<decltype(m_ScriptThreads)>();
			m_RunScriptThreads = ptr.sub(31).as<decltype(m_RunScriptThreads)>();
		});
		//Get Swapchain Device (Used for getting the swapchain from GTA 5) (Hooking\Main)
		g_mainBatch.add("GSD", "48 8B 0D ? ? ? ? 48 8B 01 44 8D 43 01 33 D2 FF 50 40 8B C8", [=](memory ptr) {
			m_GetSwapchainDevice = ptr.add(3).rip().as<decltype(m_GetSwapchainDevice)>();
		});
		//Script Globals (Used for global handles) (Hooking\Offsets)
		g_mainBatch.add("SG", "48 8D 15 ? ? ? ? 4C 8B C0 E8 ? ? ? ? 48 85 FF 48 89 1D", [=](memory ptr) {
			m_SciptGlobals = ptr.add(3).rip().as<decltype(m_SciptGlobals)>();
		});
		//Fix Vectors (Used for fixing vectors because of some odd padding system, called in the invoker) (RAGE\Invoker)
		g_mainBatch.add("FV", "83 79 18 00 48 8B D1 74 4A FF 4A 18 48 63 4A 18 48 8D 41 04 48 8B 4C CA", [=](memory ptr) {
			m_FixVectors = ptr.as<decltype(m_FixVectors)>();
		});
		//Native Return Address (Used for changing and storing the native return address, called in the invoker) (RAGE\Invoker)
		g_mainBatch.add("NRA", "FF E3", [=](memory ptr) {
			m_NativeReturnAddress = ptr.as<decltype(m_NativeReturnAddress)>();
		});
		//Player Mgr (Used for getting all players in a session, very useful) (RAGE\CPlayerMgr)
		g_mainBatch.add("NPM", "48 8B 0D ? ? ? ? 8A D3 48 8B 01 FF 50 ? 4C 8B 07 48 8B CF", [=](memory ptr) {
			m_NetworkPlayerMgr = ptr.add(3).rip().as<decltype(m_NetworkPlayerMgr)>();
		});
		//Get Label Text (Used for getting labels and replacing them) (RAGE\Labels)
		g_mainBatch.add("GLT", "75 ? E8 ? ? ? ? 8B 0D ? ? ? ? 65 48 8B 04 25 ? ? ? ? BA ? ? ? ? 48 8B 04 C8 8B 0C 02 D1 E9", [=](memory ptr) {
			m_GetLabelText = ptr.sub(19).as<decltype(m_GetLabelText)>();
		});
		//Read Bitbuffer Array
		g_mainBatch.add("RBA", "48 89 5C 24 ? 57 48 83 EC 30 41 8B F8 4C", [=](memory ptr) {
			m_ReadBitbufArray = ptr.as<decltype(m_ReadBitbufArray)>();
		});
		//Read Bitbuffer WORD/DWORD
		g_mainBatch.add("RBD", "48 89 74 24 ? 57 48 83 EC 20 48 8B D9 33 C9 41 8B F0 8A", [=](memory ptr) {
			m_ReadBitbufDword = ptr.sub(5).as<decltype(m_ReadBitbufDword)>();
		});
		//Sync Can Apply (Used for blocking model/object syncs) (Network\Protections)
		g_mainBatch.add("SCA", "49 8B CE FF 50 70 84 C0 74 31 33 FF", [=](memory ptr) {
			m_SyncCanApply = ptr.sub(44).as<decltype(m_SyncCanApply)>();
		});
		//Convert Handle (Used for converting classes to usable handles) (Network\Protections)
		g_mainBatch.add("CH", "49 8B CE E8 ? ? ? ? 49 8D 76 20", [=](memory ptr) {
			m_ConvertHandle = ptr.add(4).rip().as<decltype(m_ConvertHandle)>();
		});
		//Network Event Handler (Used for blocking network events) (Network\Protections)
		g_mainBatch.add("NEH", "66 41 83 F9 ? 0F 83 ? ? ? ?", [=](memory ptr) {
			m_NetworkEventHandler = ptr.as<decltype(m_NetworkEventHandler)>();
		});
		//Send Event Ack (Used for acknowledging network events, used to stop events from being sent and avoid overfilling the buffer) (Network\Protections)
		g_mainBatch.add("SEA", "48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 20 80 7A", [=](memory ptr) {
			m_SendEventAck = ptr.sub(5).as<decltype(m_SendEventAck)>();
		});
		//Hwnd (Used for getting window functions, and checking which wnd is which) (Hooking\Renderer)
		m_Hwnd = FindWindowA("grcWindow", nullptr);
		if (!m_Hwnd) {
			throw std::runtime_error("Failed to find the game's window.");
		}
		else {
			g_mainBatch.increaseCount();
			logEntry(LogColor::Grey, "Pattern Scanning", "Found pattern HWND!");
		}
		g_mainBatch.run("GTA5.exe");
	}
}