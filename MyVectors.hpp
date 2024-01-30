#pragma once

#include <cstdint>
#include <random>

namespace vec {
	struct vec2i {
		int x;
		int y;

		vec2i() : x(0), y(0) {}
		vec2i(int _x, int _y) : x(_x), y(_y) {}
	};

	struct vec2u8 {
		std::uint8_t x;
		std::uint8_t y;

		vec2u8() : x(0), y(0) {}
		vec2u8(std::uint8_t _x, std::uint8_t _y) : x(_x), y(_y) {}
	};

	struct vec3i {
		int x;
		int y;
		int z;

		vec3i() : x(0), y(0), z(0) {}
		vec3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
	};

	struct vec3u8 {
		std::uint8_t x;
		std::uint8_t y;
		std::uint8_t z;

		vec3u8() : x(0), y(0), z(0) {}
		vec3u8(std::uint8_t _x, std::uint8_t _y, std::uint8_t _z) : x(_x), y(_y), z(_z) {}
	};
}

namespace mymath {
	static inline uint8_t min(uint8_t _x, uint8_t _y) {
		return _x < _y ? _x : _y;
	}

	static inline uint8_t max(uint8_t _x, uint8_t _y) {
		return _x > _y ? _x : _y;
	}

	static inline uint8_t abs(uint8_t _x) {
		return _x > 0 ? _x : -_x;
	}
}