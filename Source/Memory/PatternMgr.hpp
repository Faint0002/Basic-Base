#pragma once
#include "../Includes.hpp"

namespace Base::Core::Mem {
	class memory {
	public:
		memory() = default;
		~memory() = default;
	public:
		memory(void* hand = NULL) { m_ptr = hand; }
		memory(std::uintptr_t hand = NULL) { m_ptr = (void*)hand; }
		template <typename type> std::enable_if_t<std::is_pointer<type>::value, type> as() { return static_cast<type>(m_ptr); }
		template <typename type> std::enable_if_t<std::is_lvalue_reference<type>::value, type> as() { return *static_cast<std::add_pointer_t<std::remove_reference_t<type>>>(m_ptr); }
		template <typename type> std::enable_if_t<std::is_same<type, std::uintptr_t>::value, type> as() { return reinterpret_cast<type>(m_ptr); }
		memory add(int offset) { return memory(as<std::uintptr_t>() + offset); }
		memory sub(int offset) { return memory(as<std::uintptr_t>() - offset); }
		memory rip(int offset = 4) { return add(as<std::int32_t&>()).add(offset); }
	public:
		operator bool() { return m_ptr != nullptr; }
	public:
		void* m_ptr;
	};
	class hMod {
	public:
		hMod(HMODULE hMod);
		hMod(std::string name);
		memory begin();
		memory end();
		memory exportFunc(std::string proc);
	private:
		memory m_begin;
		memory m_end;
		size_t m_size;
		std::string m_name;
	};
	struct element { uint8_t m_data; bool m_wildcard; };
	class patternScan {
	public:
		patternScan(const char* pattern);
	public:
		memory scan(hMod region = hMod(nullptr));
	public:
		std::vector<element> m_elements;
	};
	class patternBatch {
	public:
		struct ptrEntry {
			std::string m_name;
			patternScan m_pattern;
			std::function<void(memory)> m_callback;
			ptrEntry(std::string name, patternScan pattern, std::function<void(memory)> callback) : m_name(name), m_pattern(std::move(pattern)), m_callback(std::move(callback)) {}
		};
		void increaseCount(int count = 1) { ptrCount += count, totalPtrCount += count; }
		int ptrCount = 0;
		int totalPtrCount = 0;
	public:
		void add(std::string name, patternScan pattern, std::function<void(memory)> callback);
		void run(std::string moduleName);
	private:
		std::vector<ptrEntry> m_patterns;
	};
}
inline Base::Core::Mem::patternBatch g_mainBatch;