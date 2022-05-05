#pragma once
//Disable Warings
#pragma warning(disable : 4996)
//Window Includes
#include <Windows.h>
#include <vadefs.h>
#include <stack>
#include <winsdkver.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <sdkddkver.h>
#include <winternl.h>
#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <Psapi.h>
#include <timeapi.h>
#include <time.h>
#include <xstring>
#include <functional>
#include <cassert>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <atomic>
#include <chrono>
#include <deque>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <type_traits>
#pragma comment(lib, "ws2_32.lib")

//Menu Includes
#include <MinHook.h>
//DX Includes
#include <DXGi.h>
#include <D3D11.h>
#include <D3DCompiler.h>
//ImGui Includes
#include "ImGui/imgui.h"
#include "ImGui/imconfig.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
//FMT Includes
#define FMT_HEADER_ONLY
#include "FMT/format.h"
#include "FMT/format-inl.h"
//ImGui Defines
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//Namespace Defines
using namespace std::chrono_literals;
namespace fs = std::filesystem;
//Global Base::Core Namespace
namespace Base::Core {
	inline HMODULE g_hmModule = 0;
	inline HANDLE g_hModuleHandle = 0;
	inline DWORD g_dwMainThreadID = 0;
	inline bool g_Running = true;
	inline std::uint32_t g_SelectedPlayer;
}