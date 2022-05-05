#include "Hooking.hpp"
#include "LabelMgr.hpp"

namespace Base::Core::Mem {
	static std::unique_ptr<detours> detourHndle;
	void Hooking::Enable() {
		detourHndle = std::make_unique<detours>();
		detourHndle->enable();
		g_LabelManager.AddLabel("PM_GO", "Poccaking: Very Nice!");
		g_LabelManager.AddLabel("PM_ENTER_MP", "Poccaking: Very Nice!");
	}
	void Hooking::Disable() {
		detourHndle->disable();
		detourHndle.release(); detourHndle.reset();
	}
	void* Hooking::ConvertThreadToFiber(void* param) {
		if (IsThreadAFiber())
			return GetCurrentFiber();
		return detourHndle->m_convertThreadToFiberHook.getOg<decltype(&ConvertThreadToFiber)>()(param);
	}
	bool Hooking::RunScriptThreads(std::uint32_t opsToExecute) {
		if (g_Running)
			g_fbrMgr.scrTick(NETWORK::NETWORK_IS_SESSION_ACTIVE() ? "freemode" : "main_persistent"); //I really should add a check to see if the scr is active
		return detourHndle->m_runScriptThreadsHook.getOg<functionTypes::runScriptThreadsT>()(opsToExecute);
	}
	const char* Hooking::GetLabelText(void* unk, const char* label) {
		auto ret = detourHndle->m_getLabelTextHook.getOg<functionTypes::getLabelTextT>()(unk, label);
		auto setLbl = g_LabelManager.SetLabel(label, ret);
		if (setLbl != ret)
			return setLbl.c_str();
		return ret;
	}
}