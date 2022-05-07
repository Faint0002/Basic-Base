#include "Hooking.hpp"
#include "LabelMgr.hpp"
#include "Protections.hpp"

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
			g_fbrMgr.scrTick(NETWORK::NETWORK_IS_SESSION_ACTIVE() ? "freemode" : "main_persistent");
		return detourHndle->m_runScriptThreadsHook.getOg<functionTypes::runScriptThreadsT>()(opsToExecute);
	}
	const char* Hooking::GetLabelText(void* unk, const char* label) {
		if (g_Running) {
			if (auto lbl = g_LabelManager.m_LabelArray.find(label); lbl != g_LabelManager.m_LabelArray.end())
				return lbl->second.c_str();
		}
		return detourHndle->m_getLabelTextHook.getOg<functionTypes::getLabelTextT>()(unk, label);
	}
	bool Hooking::ScriptedGameEvent(CScriptedGameEvent* sge, CNetGamePlayer* sender) {
		auto args = sge->m_args;
		auto argSize = sge->m_args_size;
		auto senderName = sender->get_name();
		auto isRecieverYourself = (*g_Pointers->m_NetworkPlayerMgr)->m_local_net_player == sge->m_target_player;
		if (isRecieverYourself) {
			for (auto& evnt : Protections::m_scriptEvents) {
				if (args[0] == evnt.eventHash && *evnt.eventBlockToggle) {
					logEntry(LogColor::White, "SE Protections", "{} has tried to send the event '{}'", senderName, evnt.eventName);
					return true;
				}
			}
		}
		return detourHndle->m_scriptedGameEventHook.getOg<functionTypes::scriptedGameEventT>()(sge, sender);
	}
	bool Hooking::SyncCanApply(rage::netSyncTree* netSyncTree, rage::netObject* netObject) {
		auto networkMgr = *g_Pointers->m_NetworkPlayerMgr;
		auto syncTree = netSyncTree->m_sync_tree_node;
		if (networkMgr != nullptr) {
			auto sender = networkMgr->m_player_list[netObject->m_owner_id];
			if (sender != nullptr) {
				if (sender->is_valid()) {
					auto senderName = std::string(sender->get_name());
					auto gameObj = netObject->GetGameObject();
					if (gameObj != nullptr) {
						auto modelInfo = gameObj->m_model_info;
						for (auto& mdl : Protections::m_crashModels) {
							if (modelInfo->m_model == mdl.modelHash && *mdl.modelBlockToggle) {
								logEntry(LogColor::White, "Model Protections", "{} tried to send the crash object '{}'", senderName, mdl.modelName);
								return false;
							}
						}
					}
				}
			}
		}
		return detourHndle->m_syncCanApplyHook.getOg<functionTypes::syncCanApplyT>()(netSyncTree, netObject);
	}
}