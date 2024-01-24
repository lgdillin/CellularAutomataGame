#pragma once

#include <glm/glm.hpp>

#include "MyVectors.hpp"

enum Material {
	EMPTY,
	SAND
};

class Particle {
public:
	Particle() {
		m_material = EMPTY;
		m_color = { 0, 0, 0 };
		m_velocity = { 0.0f, 0.0f };
		m_position = { 0, 0 };
	};
	~Particle() {};

	virtual void update() {}

	Material m_material;
	//glm::vec3 m_color;
	//glm::vec2 m_position;
	vec::vec3i m_color;
	vec::vec2i m_position;
	glm::vec2 m_velocity;
};