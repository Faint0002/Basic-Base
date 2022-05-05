#include "PatternMgr.hpp"
#include "../Logger.hpp"

namespace Base::Core::Mem {
	hMod::hMod(HMODULE hMod) : m_begin(hMod), m_end(nullptr), m_size(0) {
		auto dosHeader = memory(m_begin).as<IMAGE_DOS_HEADER*>();
		auto ntHeader = memory(m_begin).add(dosHeader->e_lfanew).as<IMAGE_NT_HEADERS*>();
		m_size = ntHeader->OptionalHeader.SizeOfImage;
		m_end = memory(m_begin.add(m_size));
	}
	hMod::hMod(std::string name) : hMod(GetModuleHandleA(name.c_str())) {
	}
	memory hMod::begin() {
		return m_begin;
	}
	memory hMod::end() {
		return m_end;
	}
	memory hMod::exportFunc(std::string proc) {
		return memory(GetProcAddress(m_begin.as<HMODULE>(), proc.c_str()));
	}
	patternScan::patternScan(const char* pattern) {
		auto isHex = [=](char c) {
			switch (std::toupper(c)) {
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
			if (*pattern == '?') { m_elements.push_back({ {}, true }); continue; }
			if (*(pattern + 1) && isHex(*pattern) && isHex(*(pattern + 1))) {
				char str[3] = { *pattern, *(pattern + 1), '\0' };
				auto data = std::strtol(str, nullptr, 16);
				m_elements.push_back({ static_cast<std::uint8_t>(data), false });
			}
		} while (*(pattern++));
	}
	memory patternScan::scan(hMod region) {
		auto compareMemory = [](std::uint8_t* data, element* elem, std::size_t num) -> bool {
			for (auto i = 0; i < num; i++) {
				if (!elem[i].m_wildcard) {
					if (data[i] != elem[i].m_data)
						return false;
				}
			}
			return true;
		};
		for (auto i = region.begin().as<std::uintptr_t>(), end = region.end().as<std::uintptr_t>(); i != end; ++i) {
			if (compareMemory(reinterpret_cast<std::uint8_t*>(i), m_elements.data(), m_elements.size())) {
				return memory(i);
			}
		}
		return nullptr;
	}
	void patternBatch::add(std::string name, patternScan pattern, std::function<void(memory)> callback) {
		m_patterns.emplace_back(name, pattern, callback);
	}
	void patternBatch::run(std::string moduleName) {
		auto mod = hMod(moduleName);
		for (auto& entry : m_patterns) {
			totalPtrCount++;
			if (auto result = entry.m_pattern.scan(mod)) {
				ptrCount++;
				std::invoke(std::move(entry.m_callback), result);
				if (!entry.m_name.empty()) {
					auto base = uintptr_t(mod.begin());
					logEntry(LogColor::Grey, "Pattern Scanning", "Found pattern {}!", entry.m_name);
				}
			}
			else {
				logEntry(LogColor::Grey, "Pattern Scanning", "Failed to find pattern {}!", entry.m_name);
			}
		}
		logEntry(LogColor::Grey, "Pattern Scanning", "Found {}/{} patterns.", ptrCount, totalPtrCount);
		m_patterns.clear();
	}
}