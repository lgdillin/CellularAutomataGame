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
	static int randInt(int _max) {
		return rand() % (_max + 1);
	}

	static inline uint8_t min(uint8_t _x, uint8_t _y) {
		return _x < _y ? _x : _y;
	}

	static inline uint8_t max(uint8_t _x, uint8_t _y) {
		return _x > _y ? _x : _y;
	}

	static inline uint8_t abs(uint8_t _x) {
		return _x > 0 ? _x : -_x;
	}

	static inline uint16_t clampu16(uint16_t _x, int _lo, int _hi) {
		return max(min(_x, _hi), _lo);
	}

	static inline uint8_t lerpU8(int _x, int _imin, int _imax, int _omin, int _omax) {
		return static_cast<uint8_t>((_x - _imin) * (_omax - _omin)
			/ (_imax - _imin) + _omin);
	}

	static inline uint8_t lerp(int _a, int _b, int _t) {
		return (1 - _t) * _a + _t * _b;
	}

	//static inline uint8_t clerpu8(int _val, int _loval, int _hival, int _x1, int _x2,
	//	int _y1, int _y2
	//) {
	//	_val = clampu16(_val, _loval, _hival);
	//	return (_val - _x1) * (_y2 - _y1) / (_x2 - _x1);
	//}

	static inline uint8_t cxlerpu8(int _val, int _x1, int _x2,
		int _y1, int _y2
	) {
		uint16_t c = clampu16(_val, _x1 + 1, _x2 - 1);
		uint8_t out = static_cast<uint8_t>(_y1 + (c - _x1) * (float)(_y2 - _y1) / (_x2 - _x1));
		return out;
	}

	static inline uint8_t clampu8(int _x) {
		return max(min(_x, 255), 0);
	}

	static inline uint8_t clampu8(uint8_t _x, uint8_t _lo, uint8_t _hi) {
		return max(min(_x, _hi), _lo);
	}
}