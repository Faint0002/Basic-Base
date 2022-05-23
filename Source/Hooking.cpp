#include "Hooking.hpp"
#include "LabelMgr.hpp"
#include "Protections.hpp"
#include "Namespaces/BitBuffer.hpp"

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
	bool Hooking::NetworkEventHandler(std::int64_t* eventMgr, CNetGamePlayer* source, CNetGamePlayer* target, uint16_t id, int idx, int handledBitset, int64_t bitBufSize, int64_t bitBuf) {
		auto eventName = *(char**)(eventMgr + 8i64 * id + 243376);
		auto buffer = std::make_unique<rage::datBitBuffer>((void*)bitBuf, (uint32_t)bitBufSize);
		if (eventName == nullptr || source == nullptr || source->m_player_id < 0 || source->m_player_id >= 32) {
			g_Pointers->m_SendEventAck(eventMgr, source, target, id, handledBitset);
			return true;
		}
		switch (id) {
		case eRockstarEvent::SCRIPTED_GAME_EVENT: {
			auto sge = CScriptedGameEvent();
			buffer->ReadDword(&sge.m_args_size, 32);
			if (sge.m_args_size <= 0x1AF)
				buffer->ReadArray(&sge.m_args, 8 * sge.m_args_size);
			for (auto& evnt : Protections::m_scriptEvents) {
				if (sge.m_args[0] == evnt.eventHash && *evnt.eventBlockToggle) {
					logEntry(LogColor::White, "SE Protections", "{} has tried to send the event '{}'", source->get_name(), evnt.eventName);
					g_Pointers->m_SendEventAck(eventMgr, source, target, id, handledBitset);
					return false;
				}
			}
			buffer->Seek(0);
		} break;
		case eRockstarEvent::NETWORK_INCREMENT_STAT_EVENT: {
			rage::joaat_t stat{};
			buffer->ReadDword(&stat, 32);
			std::int32_t amount{};
			buffer->ReadInt32(&amount, 32);
			for (auto& evnt : Protections::m_reportHashes) {
				if (stat == evnt.modelHash && *evnt.modelBlockToggle) {
					logEntry(LogColor::White, "Report Protections", "{} has tried to send the report '{:X}' (ammount: %i)", source->get_name(), evnt.modelHash, amount);
					g_Pointers->m_SendEventAck(eventMgr, source, target, id, handledBitset);
					return false;
				}
			}
			buffer->Seek(0);
		} break;
		}
		return detourHndle->m_getLabelTextHook.getOg<functionTypes::networkEventHandlerT>()(eventMgr, source, target, id, idx, handledBitset, bitBufSize, bitBuf);
	}
	bool Hooking::SyncCanApply(rage::netSyncTree* netSyncTree, rage::netObject* netObject) {
		if (NETWORK::NETWORK_IS_SESSION_ACTIVE()) {
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
		}
		return detourHndle->m_syncCanApplyHook.getOg<functionTypes::syncCanApplyT>()(netSyncTree, netObject);
	}
}