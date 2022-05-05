#pragma once
#include <stdint.h>
#include <cstdint>
#include <stdio.h>
#include <iostream>
#include <vadefs.h>
#include <fstream>
#include <consoleapi.h>
#include <filesystem>
#include <cstdarg>

namespace logger {
	std::ofstream m_output;
	std::ofstream m_file;
	inline void do_send_log(const char* type, const char* format, std::va_list args) {
		auto time = std::time(nullptr);
		auto tm = std::localtime(&time);
		char prefix[64] = {};
		std::snprintf(prefix, sizeof(prefix) - 1, "[%02d:%02d:%02d] [%s] ", tm->tm_hour, tm->tm_min, tm->tm_sec, type);
		std::size_t messageLength = std::vsnprintf(nullptr, 0, format, args) + 1;
		auto messageBuffer = std::make_unique<char[]>(messageLength);
		std::uninitialized_fill_n(messageBuffer.get(), messageLength, '\0');
		std::vsnprintf(messageBuffer.get(), messageLength, format, args);
		m_file << prefix << messageBuffer.get() << std::endl;
		m_output << prefix << messageBuffer.get() << std::endl;
		messageBuffer.release();
	}
	inline void info(const char* format, ...) {
		std::va_list args{};
		va_start(args, format);
		do_send_log("Info", format, args);
		va_end(args);
	}
	inline void do_log_thread() {
		if (!AttachConsole(GetCurrentProcessId())) {
			AllocConsole();
		}
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		SetConsoleTitleA("Foxy Updating System");
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		m_output.open("CONOUT$");
		std::filesystem::path filePath;
		try {
			filePath.append("C:\\Foxy\\Dumps");
			if (!std::filesystem::exists(filePath))
				std::filesystem::create_directory(filePath);
			filePath.append("FoxyDumping.log");
		}
		catch (std::filesystem::filesystem_error const&) {
			logger::info("std::filesystem error!");
		}
		m_file.open(filePath, std::ios_base::out | std::ios_base::app);
	}
	inline void remove_log_thread() {
		fclose(stdout);
		FreeConsole();
	}
}