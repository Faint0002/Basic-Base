#pragma once
#include "Pointers.hpp"

namespace Base::Core {
	class globals {
	private:
		uint64_t m_index;
		static void* getScriptGlobal(uint64_t index) { return g_Pointers->m_SciptGlobals[index >> 18 & 0x3F] + (index & 0x3FFFF); }
		static void* getLocalGlobal(PVOID stack, uint64_t index) { return reinterpret_cast<uintptr_t*>(uintptr_t(stack) + (index * sizeof(uintptr_t))); }
	public:
		globals(uint64_t index) {
			m_index = index;
		}
		globals at(uint64_t index) {
			return globals(m_index + index);
		}
		globals at(uint64_t index, uint64_t size) {
			return at(1 + (index * size));
		}
		//Script Globals
		template <typename T> std::enable_if_t<std::is_pointer<T>::value, T> as() {
			return (T)getScriptGlobal(m_index);
		}
		//Local Globals
		template <typename T> std::enable_if_t<std::is_pointer<T>::value, T> asLocal(PVOID stack) {
			return (T)getLocalGlobal(stack, m_index);
		}
	};
}