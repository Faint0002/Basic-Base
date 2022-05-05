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
	};
	class detours {
	public:
		detours() :
			m_convertThreadToFiberHook("CTTF", Mem::hMod("kernel32.dll").exportFunc("ConvertThreadToFiber").as<void*>(), Hooking::ConvertThreadToFiber),
			m_runScriptThreadsHook("RST", g_Pointers->m_RunScriptThreads, Hooking::RunScriptThreads),
			m_getLabelTextHook("GLT", g_Pointers->m_GetLabelText, Hooking::GetLabelText)
		{}
	public:
		void enable() {
			m_convertThreadToFiberHook.enable();
			m_runScriptThreadsHook.enable();
			m_getLabelTextHook.enable();
		}
		void disable() {
			m_convertThreadToFiberHook.disable();
			m_runScriptThreadsHook.disable();
			m_getLabelTextHook.disable();
		}
	public:
		detouring m_convertThreadToFiberHook;
		detouring m_runScriptThreadsHook;
		detouring m_getLabelTextHook;
	};
}

inline Base::Core::Mem::Hooking g_Hooking;