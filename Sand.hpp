#pragma once

#include "Particle.hpp"

class Sand : public Particle {
public:
	Sand() {
		m_material = SAND;
		m_color = { 255, 255, 0 };

	}
	~Sand() {};

	void update() override {

	}
};