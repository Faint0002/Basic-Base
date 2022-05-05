#pragma once
#include "Includes.hpp"

enum LogColor : std::uint16_t {
	Red = FOREGROUND_RED,
	Green = FOREGROUND_GREEN,
	Blue = FOREGROUND_BLUE,
	Intensify = FOREGROUND_INTENSITY,
	White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Grey = FOREGROUND_INTENSITY,
	LightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
	LightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	LightBlue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
};
inline LogColor operator|(LogColor a, LogColor b) {
	return static_cast<LogColor>(static_cast<std::underlying_type_t<LogColor>>(a) | static_cast<std::underlying_type_t<LogColor>>(b));
}
namespace Base::Window {
	class Log {
	public:
		Log() = default;
		~Log() = default;
	public:
		void Initialize();
		void Shutdown();
		template <typename ...Args>
		void RawEntry(LogColor color, Args const&... args);
		template <typename ...Args>
		void SendLogEntry(LogColor color, const char* prefix, std::string_view format, Args const&... args) {
			auto Msg = fmt::format(format, args...);
			auto TimeSinceEpoch = std::time(nullptr);
			auto CurrentTime = std::localtime(&TimeSinceEpoch);
			auto ConsoleTimestamp = fmt::format("{:0>2}:{:0>2}:{:0>2} | ", CurrentTime->tm_hour, CurrentTime->tm_min, CurrentTime->tm_sec);
			auto FileTimestamp = fmt::format("{}-{}-{} | {:0>2}:{:0>2}:{:0>2} | ", CurrentTime->tm_year + 1900, CurrentTime->tm_mon + 1, CurrentTime->tm_mday, CurrentTime->tm_hour, CurrentTime->tm_min, CurrentTime->tm_sec);
			this->SendToConsole(color, ConsoleTimestamp, prefix, " | ", Msg, "\n");
			this->SendToFile(FileTimestamp, prefix, " | ", Msg, "\n");
			this->m_Messages.push_back(fmt::format("{} {} | {}\n", ConsoleTimestamp, prefix, Msg));
		}
		template<typename ...Args>
		void SendToConsole(LogColor color, Args const & ...args);
		template<typename ...Args>
		void SendToFile(Args const & ...args);
	public:
		std::vector<std::string> m_Messages;
		fs::path m_FilePath;
		std::ofstream m_File;
		std::ofstream m_Console;
		std::ifstream m_Input;
	};
}
inline std::unique_ptr<Base::Window::Log> g_Logger;

#define logEntry(color, prefix, format, ...) (g_Logger->SendLogEntry(color, prefix, format, __VA_ARGS__))