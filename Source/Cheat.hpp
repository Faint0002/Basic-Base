#pragma once
#include "Includes.hpp"
#include "Logger.hpp"
#include "Pointers.hpp"
#include "Native Invoker.hpp"
#include "Hooking.hpp"
#include "Script.hpp"
#include "Render.hpp"
#include "Gui.hpp"

namespace Base::Core {
	class MainEntry {
	public:
		MainEntry() = default;
		~MainEntry() = default;
	public:
		void Initialize();
		void Uninitialize();
	};
}
inline std::unique_ptr<Base::Core::MainEntry> g_MainEntry;