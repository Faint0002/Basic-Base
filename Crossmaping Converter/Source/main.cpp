#include "main.h"
#include "memscanner.h"
#include "logger.h"
#include "pointers.h"
#include "crossmap.h"
#include <urlmon.h>
#pragma comment(lib, "urlmon")
#include <wininet.h>
using namespace std::chrono_literals;
namespace fs = std::filesystem;
std::string gameVersion = "1.58";
std::string link = "https://raw.githubusercontent.com/Maestro-1337/GTA-V-" + gameVersion + "-Crossmap/master/Crossmap.h";
std::unordered_map<rage::scrNativeHash, rage::scrNativeHandler> m_handlerCache;
void getHandlers() {
	fs::path m_crossMap("C:\\Foxy\\Crossmap.txt");
	std::string m_stream;
	std::string m_map1;
	std::string m_map2;
	std::ifstream file;
	if (fs::exists(m_crossMap)) {
		file.open(m_crossMap.c_str());
		while (std::getline(file, m_stream)) {
			auto m_hashEnding1 = m_stream.find_first_of(",");
			auto m_hashEnding2 = m_stream.find_last_of(" },") - 2;
			auto m_hashPosition1 = m_stream.find_first_of("0x");
			auto m_hashPosition2 = m_hashEnding1 + 2;
			m_map1 = m_stream.substr(m_hashPosition1, m_hashEnding1 - m_hashPosition1);
			m_map2 = m_stream.substr(m_hashPosition2, m_hashEnding2 - m_hashPosition2);
			m_resovledCrossmapping.push_back(NativeMapping(std::stoull(m_map1, nullptr, 0), std::stoull(m_map2, nullptr, 0)));
		}
	}
	for (const auto& mapping : m_resovledCrossmapping) {
		rage::scrNativeHandler handler = g_pointers.gta_get_native_handler(
			g_pointers.gta_native_registration_table, mapping.m_New);
		m_handlerCache.emplace(mapping.m_Old, handler);
	}
}
struct NativeContext : public rage::scrNativeCallContext {
	NativeContext() {
		Reset();
	}
	void Reset() {
		m_Return = &m_ReturnData[0];
		m_ArgCount = 0;
		m_Args = &m_ArgumentData[0];
		m_DataCount = 0;
		std::fill(std::begin(m_VectorSpace), std::end(m_VectorSpace), '\0');
		std::fill(std::begin(m_ReturnData), std::end(m_ReturnData), 0);
		std::fill(std::begin(m_ArgumentData), std::end(m_ArgumentData), 0);
	}
	template <typename T>
	void Push(T value) {
		static_assert(sizeof(T) <= 8);
		std::uintptr_t data{};
		*(T*)(&data) = value;
		m_ArgumentData[m_ArgCount++] = data;
	}
	template <typename T>
	T GetReturnValue() { return *static_cast<T*>(m_Return); }
	template <>
	void GetReturnValue<void>() {}
private:
	std::uintptr_t m_ReturnData[10] = {};
	std::uintptr_t m_ArgumentData[100] = {};
};
NativeContext m_context;
const char* InvokeGameVer() {
	std::uint64_t GGOHash = 0xFCA9373EF340AC0A;
	m_context.Reset();
	if (auto cache = m_handlerCache.find(GGOHash); cache != m_handlerCache.end()) {
		rage::scrNativeHandler handler = cache->second;
		handler(&m_context);
	}
	return m_context.GetReturnValue<const char*>();
}
void parseEntryPoints() {
	std::ofstream m_output;
	fs::path m_path;
	DWORD64 m_moduleBase = (DWORD64)GetModuleHandleA(NULL);
	getHandlers();
	auto m_nativeCache = m_handlerCache;
	size_t m_nativeCount = m_handlerCache.size();
	std::vector<std::string> m_fileBeginText = {
		"#pragma once",
		"#include <cstdint>",
		"#define nativeCount " + std::to_string(m_nativeCount),
		"//Crossmapping dumper created by Vali#0004 | Format is { 0x(DAYONE_HASH), 0x(ENTRYPOINT) }, | Number of handlers: " + std::to_string(m_nativeCount) + " | Game Update: " + std::string(InvokeGameVer()) + "\n",
		"struct nativeTable {",
		"	std::uint64_t m_nativeHandleHash;",
		"	std::uint64_t m_nativeEntryHash;",
		"};\n",
		"inline nativeTable g_nativeTable[nativeCount] {",
	};
	std::string m_fileEndText = "};";
	m_path.append("C:\\Foxy\\Dumps");
	if (!fs::exists(m_path))
		fs::create_directory(m_path);
	m_path.append("rageHashMap.hpp");
	m_output.open(m_path, std::ios::binary | std::ios::trunc);
	for (auto m_text : m_fileBeginText) {
		m_output << std::string(m_text + "\n").c_str();
	}
	for (auto m_entry : m_handlerCache) {
		char m_crossmapLineText[200] = {};
		DWORD64 m_calculatedEntryPoint = ((DWORD64)m_nativeCache[m_entry.first]) - m_moduleBase;
		sprintf_s(m_crossmapLineText, "	{ 0x%llX, 0x%llX },", m_entry.first, m_calculatedEntryPoint);
		std::string m_formattedCrossmapLineText = std::string(&m_crossmapLineText[0]) + "\n";
		m_output << m_formattedCrossmapLineText.c_str();
	}
	m_output << m_fileEndText;
	m_output.flush();
	m_output.close();
}
fs::path m_crossMap("C:\\Foxy\\Crossmap.txt");
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		g_main_thread_hmodule = module;
		g_main_thread = CreateThread(nullptr, 0, [](LPVOID module) -> DWORD {
			logger::do_log_thread();
			g_pointers.add_pointers();
			logger::info("Started dumping entry points and formatting of crossmaps...");
			if (!fs::exists(m_crossMap)) {
				logger::info("Version to grab (parsing only): ");
				std::cin >> gameVersion;
				if (!gameVersion.empty()) {
					parseEntryPoints();
					logger::info("Finished!\n Crossmap path: %s\n Dumped crossmap: %s", "C:\\Foxy\\Crossmap.txt", "C:\\Foxy\\Dumps\\rageHashMap.hpp");
				}
			}
			else {
				parseEntryPoints();
				logger::info("Finished!\n Crossmap path: %s\n Dumped crossmap: %s", "C:\\Foxy\\Crossmap.txt", "C:\\Foxy\\Dumps\\rageHashMap.hpp");
			}
			std::this_thread::sleep_for(3s);
			g_pointers.remove_pointers();
			logger::remove_log_thread();
			CloseHandle(g_main_thread);
			FreeLibraryAndExitThread(g_main_thread_hmodule, g_main_thread_id);
		}, module, g_main_thread_id, nullptr);
	}
	return TRUE;
}