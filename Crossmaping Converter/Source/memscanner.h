#pragma once
#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <libloaderapi.h>
#include <vector>

class mem {
public:
	mem(void* p = nullptr) : m_Ptr(p) {
	}
	explicit mem(std::uintptr_t p) : m_Ptr((void*)p) {
	}
	template <typename T> std::enable_if_t<std::is_pointer_v<T>, T> as() {
		return static_cast<T>(m_Ptr);
	}
	template <typename T>
	std::enable_if_t<std::is_lvalue_reference_v<T>, T> as() {
		return *static_cast<std::add_pointer_t<std::remove_reference_t<T>>>(m_Ptr);
	}
	template <typename T> std::enable_if_t<std::is_same_v<T, std::uintptr_t>, T> as() {
		return (T)m_Ptr;
	}
	template <typename T> mem add(T offset) {
		return mem(as<std::uintptr_t>() + offset);
	}
	template <typename T> mem sub(T offset) {
		return mem(as<std::uintptr_t>() - offset);
	}
	mem rip() {
		if (!m_Ptr)
			return nullptr;
		return add(as<std::int32_t&>()).add(4U);
	}
	explicit operator bool() noexcept {
		return m_Ptr;
	}
protected:
	void* m_Ptr;
};
class memregion {
public:
	explicit memregion(mem base, std::size_t size) : m_base(base), m_size(size) {
	}
	mem base() {
		return m_base;
	}
	mem end() {
		return m_base.add(m_size);
	}
	std::size_t size() {
		return m_size;
	}
	bool contains(mem p) {
		if (p.as<std::uintptr_t>() < m_base.as<std::uintptr_t>())
			return false;
		if (p.as<std::uintptr_t>() > m_base.add(m_size).as<std::uintptr_t>())
			return false;
		return true;
	}
protected:
	mem m_base;
	std::size_t m_size;
};
class module : public memregion
{
public:
	explicit module(std::nullptr_t) : module(static_cast<char*>(nullptr)) {
	}
	explicit module(const char* name) : module(GetModuleHandleA(name)) {
	}
	module(HMODULE hmod) : memregion((mem)hmod, 0) {
		auto dos_header = m_base.as<IMAGE_DOS_HEADER*>();
		auto nt_header = m_base.add(dos_header->e_lfanew).as<IMAGE_NT_HEADERS64*>();
		m_size = nt_header->OptionalHeader.SizeOfImage;
	}
	IMAGE_DOS_HEADER* get_dos_headers() {
		return m_base.as<IMAGE_DOS_HEADER*>();
	}
	IMAGE_NT_HEADERS64* get_nt_headers() {
		return m_base.add(m_base.as<IMAGE_DOS_HEADER*>()->e_lfanew).as<IMAGE_NT_HEADERS64*>();
	}
private:
	template <typename TReturn, typename TOffset> TReturn translate_rva(TOffset rva) {
		return m_base.add(rva).as<TReturn>();
	}
};
class memsigscan {
public:
	struct element {
		std::uint8_t m_Data{};
		bool m_Wildcard{};
	};
	memsigscan(const char* pattern)
	{
		auto toUpper = [](char c) -> char { return c >= 'a' && c <= 'z' ? static_cast<char>(c + ('A' - 'a')) : static_cast<char>(c); };
		auto isHex = [&](char c) -> bool {
			switch (toUpper(c)) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				return true;
			default:
				return false;
			}
		};
		do {
			if (*pattern == ' ') continue;
			if (*pattern == '?') {
				m_elements.push_back(element{ {}, true });
				continue;
			}
			if (*(pattern + 1) && isHex(*pattern) && isHex(*(pattern + 1))) {
				char str[3] = { *pattern, *(pattern + 1), '\0' };
				auto data = std::strtol(str, nullptr, 16);
				m_elements.push_back(element{ static_cast<std::uint8_t>(data), false });
			}
		} while (*(pattern++));
	}
	mem scan(memregion region = module(nullptr))
	{
		auto compareMemory = [](std::uint8_t* data, element* elem, std::size_t num) -> bool {
			for (std::size_t i = 0; i < num; ++i) {
				if (!elem[i].m_Wildcard)
					if (data[i] != elem[i].m_Data)
						return false;
			}
			return true;
		};
		for (std::uintptr_t i = region.base().as<std::uintptr_t>(), end = region.end().as<std::uintptr_t>(); i != end; ++i) {
			if (compareMemory(reinterpret_cast<std::uint8_t*>(i), m_elements.data(), m_elements.size())) {
				return mem(i);
			}
		}
		return {};
	}
private:
	std::vector<element> m_elements;
};