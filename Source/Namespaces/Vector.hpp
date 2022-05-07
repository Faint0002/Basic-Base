#pragma once
#include "../Includes.hpp"

namespace rage {
	struct vector2 { float x, y; };
	struct vector3 { float x, y, z; };
	struct vector4 { float x, y, z, w; };
}

class Vector2 {
public:
	Vector2() = default;
	Vector2(float x, float y) : x(x), y(y) {}
public:
	float x;
private:
	char m_paddingx[0x04];
public:
	float y;
private:
	char m_paddingy[0x04];
};
class Vector3 {
public:
	Vector3() = default;
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
public:
	float x;
private:
	char m_paddingx[0x04];
public:
	float y;
private:
	char m_paddingy[0x04];
public:
	float z;
private:
	char m_paddingz[0x04];
};
class Vector4 {
public:
	Vector4() = default;
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
public:
	float x;
private:
	char m_paddingx[0x04];
public:
	float y;
private:
	char m_paddingy[0x04];
public: float z;
private:
	char m_paddingz[0x04];
public:
	float w;
private:
	char m_paddingw[0x04];
};