#include "Game.hpp"

Game::Game() {
	m_particles = std::vector<Particle>(10);
}

Game::~Game() {
}

void Game::initialize() {
	for (int i = 0; i < 10; ++i) {
		Sand s;
		m_particles[i] = s;
		m_particles[i].m_position = vec::vec2i(i, i);
	}
}

void Game::update() {
}
