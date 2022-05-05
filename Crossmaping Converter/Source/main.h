#pragma once
#include <locale>
#include <iostream>
#include <sstream>
#include <fstream>
#include <Windows.h>
#include <chrono>
#include <filesystem>
#include <queue>
#include <Wincrypt.h>
#include <winternl.h>
#include <stdint.h>
#include <cstdint>
#include <stdio.h>
#include <cmath>
#include <array>
#include <thread>
#include <vector>
#include <stack>
#include <unordered_map>
#include <iomanip>
#include <functional>
#include <shlobj_core.h>
#include <intrin.h>
#include <MinHook.h>
#include "nspcrage.h"
#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")
#pragma warning(disable : 4996)
#define MENU_BUILD_DATE __DATE__
#define MENU_BUILD_TIME __TIME__
#pragma message("========== Building Foxy ==========")
#pragma message("Date -> " MENU_BUILD_DATE)
#pragma message("Time -> " MENU_BUILD_TIME)
#pragma message("======================================")
using namespace std::chrono_literals;

bool g_running = true;
bool g_opened = true;
bool g_lock_input = true;
HMODULE g_main_thread_hmodule;
HANDLE g_main_thread;
DWORD g_main_thread_id;