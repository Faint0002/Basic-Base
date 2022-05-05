#pragma once
#include "Includes.hpp"
#include "Cheat.hpp"
#include "Pointers.hpp"

namespace Base::Core::Mem {
	class VMT {
	public:
		explicit VMT(void* object, std::size_t numFuncs) noexcept:
			m_Object((std::uintptr_t**)object), m_NumFuncs(numFuncs),
			m_Original(*m_Object), m_New(std::make_unique<std::uintptr_t[]>(m_NumFuncs + 1)) {
			std::copy_n(m_Original - 1, m_NumFuncs + 1, m_New.get());
		}
		~VMT() noexcept = default;
		void Enable() noexcept {
			*m_Object = &m_New[1];
		}
		void Disable() noexcept {
			*m_Object = m_Original;
		}
		void Hook(void* replacement, std::size_t index) noexcept {
			m_New[index + 1] = (std::uintptr_t)replacement;
			logEntry(LogColor::Grey, "VMT Hooks", "Hooked index {} on a table of {} functions.", index, m_NumFuncs);
		}
		void Unhook(std::size_t index) noexcept {
			m_New[index + 1] = m_Original[index];
		}
		template <typename T>
		T GetOriginal(std::size_t index) noexcept {
			return (T)m_Original[index];
		}
		VMT(VMT const&) noexcept = delete;
		VMT(VMT&&) noexcept = delete;
		VMT& operator=(VMT const&) noexcept = delete;
		VMT& operator=(VMT&&) noexcept = delete;
	private:
		std::uintptr_t** m_Object;
		std::size_t m_NumFuncs;
		std::uintptr_t* m_Original;
		std::unique_ptr<std::uintptr_t[]> m_New;
	};
}