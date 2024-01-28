#pragma once

#include <glm/glm.hpp>

#include "MyVectors.hpp"

/// <summary>
/// Particle class
/// </summary>
/// <param name="m_id">
/// 0: EMPTY
/// 1: SAND
/// </param>
class Particle {
public:
	Particle() {
		m_id = 0;
		m_life = 255;
		m_temp = 70;
		m_color = { 0, 0, 0 };
	};
	~Particle() {};

	virtual void update() {}

	std::uint8_t m_id;
	std::uint8_t m_life;
	std::uint8_t m_temp;
	vec::vec3u8 m_color;
};