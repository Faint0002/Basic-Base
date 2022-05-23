#pragma once
#include "Includes.hpp"
#include "Cheat.hpp"
#include "Pointers.hpp"

namespace Base::Core::Mem {
	class detouring {
	public:
		detouring() = default;
		detouring(std::string name, void* target, void* detour) : m_name(std::move(name)), m_target(target), m_detour(detour) {
			if (auto status = MH_CreateHook(m_target, m_detour, &m_original); status == MH_OK)
				logEntry(LogColor::Grey, "Hooks", "Created hook {}.", m_name);
			else
				logEntry(LogColor::Grey, "Hooks", "Failed to create hook {}.", m_name);
		}
		~detouring() {
			if (auto status = MH_RemoveHook(m_target); status == MH_OK)
				logEntry(LogColor::Grey, "Hooks", "Removed hook {}.", m_name);
			else
				logEntry(LogColor::Grey, "Hooks", "Failed to remove hook {}.", m_name);
		}
	public:
		void enable() {
			if (auto status = MH_EnableHook(m_target); status == MH_OK)
				logEntry(LogColor::Grey, "Hooks", "Enabled hook {}.", m_name);
			else
				logEntry(LogColor::Grey, "Hooks", "Failed to enable hook {}.", m_name);
		}
		void disable() {
			if (auto status = MH_DisableHook(m_target); status == MH_OK)
				logEntry(LogColor::Grey, "Hooks", "Disabled hook {}.", m_name);
			else
				logEntry(LogColor::Grey, "Hooks", "Failed to disable hook {}.", m_name);
		}
	public:
		template <typename T>
		T getOg() {
			return T(m_original);
		}
	public:
		std::string m_name;
	public:
		void* m_target;
		void* m_detour;
		void* m_original;
	};
	class Hooking {
	public:
		Hooking() = default;
		~Hooking() = default;
	public:
		void Enable();
		void Disable();
	public:
		static void* ConvertThreadToFiber(void* param);
		static bool RunScriptThreads(std::uint32_t opsToExecute);
		static const char* GetLabelText(void* unk, const char* label);
		static bool NetworkEventHandler(std::int64_t* eventMgr, CNetGamePlayer* source, CNetGamePlayer* target, uint16_t id, int idx, int handledBitset, int64_t bitBufSize, int64_t bitBuf);
		static bool SyncCanApply(rage::netSyncTree* netSyncTree, rage::netObject* netObject);
	};
	class detours {
	public:
		detours() :
			m_convertThreadToFiberHook("CTTF", Mem::hMod("kernel32.dll").exportFunc("ConvertThreadToFiber").as<void*>(), Hooking::ConvertThreadToFiber),
			m_runScriptThreadsHook("RST", g_Pointers->m_RunScriptThreads, Hooking::RunScriptThreads),
			m_getLabelTextHook("GLT", g_Pointers->m_GetLabelText, Hooking::GetLabelText),
			m_networkEventHandlerHook("NEH", g_Pointers->m_NetworkEventHandler, Hooking::NetworkEventHandler),
			m_syncCanApplyHook("SCA", g_Pointers->m_SyncCanApply, Hooking::SyncCanApply)
		{}
	public:
		void enable() {
			m_convertThreadToFiberHook.enable();
			m_runScriptThreadsHook.enable();
			m_getLabelTextHook.enable();
			m_networkEventHandlerHook.enable();
			m_syncCanApplyHook.enable();
		}
		void disable() {
			m_convertThreadToFiberHook.disable();
			m_runScriptThreadsHook.disable();
			m_getLabelTextHook.disable();
			m_networkEventHandlerHook.disable();
			m_syncCanApplyHook.disable();
		}
	public:
		detouring m_convertThreadToFiberHook;
		detouring m_runScriptThreadsHook;
		detouring m_getLabelTextHook;
		detouring m_networkEventHandlerHook;
		detouring m_syncCanApplyHook;
	};
}

inline Base::Core::Mem::Hooking g_Hooking;