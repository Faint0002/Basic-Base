#pragma once
#include "Includes.hpp"
#include "Logger.hpp"
#include "Memory/PatternMgr.hpp"
#include "Namespaces/Namespace Rage.hpp"

namespace Base::functionTypes {
	using runScriptThreadsT = bool(*)(std::uint32_t ops_to_execute);
	using fixVectorsT = void(*)(rage::scrNativeCallContext*);
	using getLabelTextT = const char*(*)(void* unk, const char* label);
	using scriptedGameEventT = bool(*)(CScriptedGameEvent* sge, CNetGamePlayer* sender);
	using readBitbufArrayT = bool(*)(rage::datBitBuffer* buffer, PVOID read, int bits, int unk);
	using readBitbufDwordT = bool(*)(rage::datBitBuffer* buffer, PVOID read, int bits);
	using syncCanApplyT = bool(*)(rage::netSyncTree* netSyncTree, rage::netObject* netObject);
}

namespace Base::Core::Mem {
	class Pointers {
	public:
		Pointers() = default;
		~Pointers() = default;
	public:
		void ScanPatterns();
	public:
		functionTypes::runScriptThreadsT m_RunScriptThreads;
		functionTypes::fixVectorsT m_FixVectors;
		functionTypes::getLabelTextT m_GetLabelText;
		functionTypes::scriptedGameEventT m_ScriptedGameEvent;
		functionTypes::readBitbufArrayT m_ReadBitbufArray;
		functionTypes::readBitbufDwordT m_ReadBitbufDword;
		functionTypes::syncCanApplyT m_SyncCanApply;
	public:
		void** m_SendChatMsgPtr;
	public:
		std::uint64_t** m_SciptGlobals;
	public:
		rage::atArray<GtaThread*>* m_ScriptThreads;
	public:
		CNetworkPlayerMgr** m_NetworkPlayerMgr;
	public:
		PVOID* m_NativeReturnAddress;
	public:
		HWND m_Hwnd;
	public:
		IDXGISwapChain** m_GetSwapchainDevice;
	};
}
inline std::unique_ptr<Base::Core::Mem::Pointers> g_Pointers;