#include "Native Invoker.hpp"
#include "Memory/PatternMgr.hpp"

extern "C" {
	//Get the function from our asm, it works by getting a 'extern' c function from any file. Either C or asm, it goes through the build targets and gets this function
	//Call ASM | modifying our native return address, 'spoofing' it to a different dummy address and then restoring it to avoid detection.
	void callAsm(void* context, void* function, void* ret);
}
namespace Base::Core::Invoke {
	void nativeInvoker::cacheHandlers() {
		for (auto& mappedFunc : g_nativeTable) {
			auto handler = Mem::hMod("GTA5.exe").begin().add(mappedFunc.m_nativeEntryHash).as<rage::scrNativeHandler>();
			m_cache.emplace(mappedFunc.m_nativeHandleHash, handler);
		}
	}
	void nativeInvoker::beginCall() {
		m_context.reset();
	}
	void nativeInvoker::endCall(rage::scrNativeHash hash) {
		if (auto mappedFunc = m_cache.find(hash); mappedFunc != m_cache.end()) {
			auto handler = mappedFunc->second;
			if (!handler) {
				logEntry(LogColor::Red, "Invoker", "Failed to invoke native 0x{:X}", hash);
				return;
			}
			//Orginial handler, kinda just left here for debugging
			//handler(&m_Context);
			//Spoofed Handler
			callAsm(&m_context, handler, g_Pointers->m_NativeReturnAddress);
			//Fix Vectors
			g_Pointers->m_FixVectors(&m_context);
		}
		else {
			logEntry(LogColor::Red, "Invoker", "Failed to find 0x{:X} handler.", hash);
		}
	}
}