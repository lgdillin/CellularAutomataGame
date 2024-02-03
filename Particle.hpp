#pragma once

#include <glm/glm.hpp>

#include "Macros.hpp"
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
		m_id = EMPTY;
		m_updated = 0;
		m_tdiff = EMPTY_TDIFF;
		m_temp = EMPTY_BASETEMP;
		m_color = { 0, 0, 0 };
	};
	~Particle() {};

	virtual void update() {}


	std::uint8_t m_id;
	std::uint8_t m_updated;
	//std::uint16_t m_specHeat;
	std::uint16_t m_tdiff;
	std::uint16_t m_temp;
	vec::vec3u8 m_color;
};