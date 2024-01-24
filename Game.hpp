#pragma once

#include <vector>
#include <array>

#include "Particle.hpp"
#include "Sand.hpp"

#define rows 600
#define cols 800

class Game {
public:

	Game();
	~Game();

	void initialize();

	void update();

	std::vector<Particle> m_particles;
	//std::vector<std::vector<Particle>> m_particles;
	//std::array<std::array<Particle, cols>, rows> m_particles;
private:

};