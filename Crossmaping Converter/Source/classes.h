#pragma once
#include <windows.h>

namespace std {
	class vector3 {
	public:
		std::float_t x{};
		std::float_t y{};
		std::float_t z{};
	};
}

class Vector3 {
public:
	std::float_t x{};
private:
	std::uint8_t x_pad[4]{};
public:
	std::float_t y{};
private:
	std::uint8_t y_pad[4]{};
public:
	std::float_t z{};
private:
	std::uint8_t z_pad[4]{};
};

class Vector2 {
public:
	std::float_t x{};
	std::float_t y{};
};