#pragma once
#include "nspcrage.h"
#define crossmapSize 6067

struct NativeMapping {
	NativeMapping(rage::scrNativeHash oldHash, rage::scrNativeHash newHash) {
		m_Old = oldHash;
		m_New = newHash;
	}
	rage::scrNativeHash m_Old;
	rage::scrNativeHash m_New;
};

std::vector<NativeMapping> m_resovledCrossmapping;                                                                                                                                                                                                                                                                                                         