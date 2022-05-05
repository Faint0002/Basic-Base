#include "Logger.hpp"

namespace Base::Window {
	void Log::Initialize() {
		if (!AttachConsole(GetCurrentProcessId())) {
			AllocConsole();
		}
		SetConsoleTitleW(L"Basic Base v2.0");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
		m_Console.open("CONOUT$");
		m_Input.open("CONIN$");
		try {
			m_FilePath.append(std::getenv("appdata"));
			m_FilePath.append("Base");
			if (!fs::exists(m_FilePath))
				fs::create_directory(m_FilePath);
			m_FilePath.append("BaseLogger.log");
		}
		catch (std::filesystem::filesystem_error const& err) {
			logEntry(LogColor::Red, "std::filesystem", "Recieved Error: {}", err.what());
		}
		m_File.open(m_FilePath, std::ios_base::out | std::ios_base::app);
	}
	void Log::Shutdown() {
		fclose(stdout);
		FreeConsole();
	}
	template <typename ...Args>
	void Log::SendToConsole(LogColor color, Args const&... args) {
		auto ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(ConsoleHandle, static_cast<std::uint16_t>(color));
		if (m_Console) {
			((m_Console << args), ...);
			m_Console << std::flush;
		}
	}
	template <typename ...Args>
	void Log::SendToFile(Args const&... args) {
		if (m_File) {
			((m_File << args), ...);
			m_File << std::flush;
		}
	}
	template <typename ...Args>
	void Log::RawEntry(LogColor color, Args const&... args) {
		SendToConsole(color, args...);
		SendToFile(args...);
	}
}