#pragma once

#include <algorithm>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <random>

#include "Particle.hpp"
#include "Sand.hpp"

#include "Macros.hpp"

class Game {
public:

	Game();
	~Game();

	void initialize();

	void update();

	void addParticle();

	void updateSand(int _x, int _y);
	void updateWater(int _x, int _y);
	void updateFire(int _x, int _y);
	void updateSmoke(int _x, int _y);
	void updateMetal(int _x, int _y);

	void pSwap(size_t _idx1, size_t _idx2);

	void setEmpty(int _x, int _y);

	void addWall(int _x, int _y);
	void addSand(int _x, int _y);
	void addWater(int _x, int _y);
	void addFire(int _x, int _y);
	void addMetal(int _x, int _y);

	//Particle *m_particles;
	std::vector<Particle> m_particles;
	std::mt19937 m_randomGen;
	std::uniform_int_distribution<int> m_dWall;
	std::uniform_int_distribution<int> m_dSand;
	std::uniform_int_distribution<int> m_dWater;
	std::uniform_int_distribution<int> m_dFire;

	int m_mousex;
	int m_mousey;
	int m_particleChoice;

	//
	// INLINE FUNCTIONS
	// functions defined inside class definition are implicitly inline
	size_t getIndex(int _x, int _y) {
		return size_t(TEXTURE_COLS * _y + _x);
	}

	bool inBounds(int _x, int _y) {
		return (_x >= 0 && _x < TEXTURE_COLS) && (_y >= 0 && _y < TEXTURE_ROWS);
	}

	bool isEmpty(int _x, int _y) {
		return inBounds(_x, _y) && m_particles[getIndex(_x, _y)].m_id == EMPTY;
	}

	bool isWater(int _x, int _y) {
		return inBounds(_x, _y) && m_particles[getIndex(_x, _y)].m_id == WATER;
	}

	bool isSmoke(int _x, int _y) {
		return inBounds(_x, _y) && m_particles[getIndex(_x, _y)].m_id == SMOKE;
	}

	uint8_t getId(int _x, int _y) {
		return m_particles[getIndex(_x, _y)].m_id;
	}

	void avgTemps(int _idx1, int _idx2) {
		uint8_t temp1 = m_particles[_idx1].m_temp;
		uint8_t temp2 = m_particles[_idx2].m_temp;
		uint8_t avg = static_cast<uint8_t>((temp1 + temp2) / 2);

		m_particles[_idx1].m_temp = avg;
		m_particles[_idx2].m_temp = avg;
	}

	bool noNeighbors(int _x, int _y) {
		bool topLeft = isEmpty(_x - 1, _y + 1);
		bool top = isEmpty(_x, _y + 1);
		bool topRight = isEmpty(_x + 1, _y + 1);
		bool left = isEmpty(_x - 1, _y);
		bool right = isEmpty(_x + 1, _y);
		bool bottomLeft = isEmpty(_x - 1, _y - 1);
		bool bottom = isEmpty(_x, _y - 1);
		bool bottomRight = isEmpty(_x + 1, _y + 1);

		return !(topLeft && top && topRight && left && right
			&& bottomLeft && bottom && bottomRight);
	}

private:
};