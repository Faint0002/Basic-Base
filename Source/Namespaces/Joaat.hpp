#pragma once
#include "../Includes.hpp"

namespace rage {
	using joaat_t = std::uint32_t;
	inline constexpr char joaatToLower(char c) {
		return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
	}
	inline joaat_t joaat(std::string_view str) {
		joaat_t hash = 0;
		for (auto c : str) {
			hash += joaatToLower(c);
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);
		return hash;
	}
	template <std::size_t charCount>
	struct constexprJoaat {
		char data[charCount];
		template <std::size_t... indices>
		constexpr constexprJoaat(const char* str, std::index_sequence<indices...>) : data{ (str[indices])... } {}
		constexpr joaat_t operator()() {
			joaat_t hash = 0;
			for (auto i = 0; i < charCount; i++) {
				hash += joaatToLower(data[i]);
				hash += (hash << 10);
				hash ^= (hash >> 6);
			}
			hash += (hash << 3);
			hash ^= (hash >> 11);
			hash += (hash << 15);
			return hash;
		}
	};
}
#define rageJoaatImpl(str) (::rage::constexprJoaat<sizeof(str) - 1>((str), std::make_index_sequence<sizeof(str) - 1>())())
#define rageJoaat(str) (std::integral_constant<rage::joaat_t, rageJoaatImpl(str)>::value)