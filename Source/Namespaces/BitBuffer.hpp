#pragma once
#include "../Includes.hpp"
#include "../Pointers.hpp"

namespace rage {
	class datBitBuffer {
	public:
		datBitBuffer(void* data, uint32_t size) {
			m_data = data;
			m_f8 = 0;
			m_maxBit = size * 8;
			m_unkBit = 0;
			m_curBit = 0;
			m_unk2Bit = 0;
			m_flagBits = 0;
		}
		uint32_t GetPosition() {
			return m_unkBit;
		}
		bool Seek(uint32_t bits) {
			if (bits >= 0) {
				uint32_t length = (m_flagBits & 1) ? m_maxBit : m_curBit;
				if (bits <= length) {
					m_unkBit = bits;
				}
			}
			return false;
		}
		int GetDataLength() {
			int leftoverBit = (m_curBit % 8) ? 1 : 0;
			return (m_curBit / 8) + leftoverBit;
		}
		bool ReadByte(uint8_t* integer, int bits) {
			uint32_t read;
			if (g_Pointers->m_ReadBitbufDword(this, &read, bits)) {
				*integer = read;
				return true;
			}
			return false;
		}
		bool ReadWord(uint16_t* integer, int bits) {
			uint32_t read;
			if (g_Pointers->m_ReadBitbufDword(this, &read, bits)) {
				*integer = read;
				return true;
			}
			return false;
		}
		bool ReadDword(uint32_t* integer, int bits) {
			return g_Pointers->m_ReadBitbufDword(this, integer, bits);
		}
		bool ReadInt32(int32_t* integer, int bits) {
			int32_t v8;
			int32_t v9;
			if (ReadDword((uint32_t*)&v8, 1u) && ReadDword((uint32_t*)&v9, bits - 1)) {
				*integer = v8 + (v9 ^ -v8);
				return true;
			}
			return false;
		}
		bool ReadQWord(uint64_t* integer, int bits) {
			if (bits <= 32) {
				uint32_t v10;
				if (ReadDword(&v10, bits)) {
					*integer = v10;
					return true;
				}
			}
			else {
				uint32_t v10, v11;
				if (ReadDword(&v11, 32u) && ReadDword(&v10, bits - 32u)) {
					*integer = v11 | ((uint64_t)v10 << 32);
					return true;
				}
			}
			return false;
		}
		bool ReadInt64(int64_t* integer, int bits) {
			uint32_t v8;
			uint64_t v9;
			if (ReadDword(&v8, 1u) && ReadQWord(&v9, bits - 1)) {
				*integer = v8 + (v9 ^ -(int64_t)v8);
				return true;
			}
			return false;
		}
		bool ReadArray(PVOID array, int size) {
			return g_Pointers->m_ReadBitbufArray(this, array, size, 0);
		}
	public:
		void* m_data; //0x0000
		uint32_t m_f8; //0x0008
		uint32_t m_maxBit; //0x000C
		uint32_t m_unkBit; //0x0010
		uint32_t m_curBit; //0x0014
		uint32_t m_unk2Bit; //0x0018
		uint8_t m_flagBits; //0x001C
	};
}