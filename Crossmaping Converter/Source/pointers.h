#pragma once
#include "main.h"
#include "memscanner.h"
#include "logger.h"

namespace function_types {
	using get_native_handler_t = rage::scrNativeHandler(*)(rage::scrNativeRegistrationTable*, std::uint64_t);
}

class pointers {
public:
	void add_pointers() {
		logger::info("Allocating sigs...");
		gta_get_native_handler = memsigscan("48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A").scan().add(12).rip().as<decltype(gta_get_native_handler)>();
		logger::info("Allocated GTNTVEHNDLR...");
		gta_native_registration_table = memsigscan("48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A").scan().add(3).rip().as<decltype(gta_native_registration_table)>();
		logger::info("Allocated NTVEREGTBLE...");
		logger::info("Allocated all sigs!");
	}
	void remove_pointers() {
		logger::info("Nulling out sigs...");
		gta_get_native_handler = nullptr;
		logger::info("Nulled out GTNTVEHNDLR...");
		gta_native_registration_table = nullptr;
		logger::info("Nulled out NTVEREGTBLE...");
		logger::info("Nulled out all sigs!");
	}
public:
	function_types::get_native_handler_t gta_get_native_handler;
	rage::scrNativeRegistrationTable* gta_native_registration_table;
};
inline pointers g_pointers;