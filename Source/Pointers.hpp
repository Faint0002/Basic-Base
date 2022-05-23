#pragma once
#include "Includes.hpp"
#include "Logger.hpp"
#include "Memory/PatternMgr.hpp"
#include "Namespaces/Namespace Rage.hpp"

namespace Base::functionTypes {
	using runScriptThreadsT = bool(*)(std::uint32_t ops_to_execute);
	using fixVectorsT = void(*)(rage::scrNativeCallContext*);
	using getLabelTextT = const char*(*)(void* unk, const char* label);
	using readBitbufArrayT = bool(*)(rage::datBitBuffer* buffer, PVOID read, int bits, int unk);
	using readBitbufDwordT = bool(*)(rage::datBitBuffer* buffer, PVOID read, int bits);
	using syncCanApplyT = bool(*)(rage::netSyncTree* netSyncTree, rage::netObject* netObject);
	using convertHandleT = int32_t(*)(void* hndle);
	using networkEventHandlerT = bool(*)(std::int64_t* eventMgr, CNetGamePlayer* source, CNetGamePlayer* target, uint16_t id, int idx, int handledBitset, int64_t bitBufSize, int64_t bitBuf);
	using sendEventAckT = void(*)(std::int64_t* eventMgr, CNetGamePlayer* source, CNetGamePlayer* target, int idx, int handledBitset);
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
		functionTypes::readBitbufArrayT m_ReadBitbufArray;
		functionTypes::readBitbufDwordT m_ReadBitbufDword;
		functionTypes::syncCanApplyT m_SyncCanApply;
		functionTypes::convertHandleT m_ConvertHandle;
		functionTypes::networkEventHandlerT m_NetworkEventHandler;
		functionTypes::sendEventAckT m_SendEventAck;
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