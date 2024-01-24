#pragma once

namespace vec {
	struct vec2i {
		int x;
		int y;

		vec2i() : x(0), y(0) {}
		vec2i(int _x, int _y) : x(_x), y(_y) {}
	};

	struct vec3i {
		int x;
		int y;
		int z;

		vec3i() : x(0), y(0), z(0) {}
		vec3i(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
	};
}