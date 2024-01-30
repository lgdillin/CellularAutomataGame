#pragma once

#include <bitset>
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

	void updateTemp(int _x, int _y);

	void updateSand(int _x, int _y);
	void updateWater(int _x, int _y);
	void updateFire(int _x, int _y);
	void updateSmoke(int _x, int _y);
	void updateMetal(int _x, int _y);
	void updateSteam(int _x, int _y);
	void updateTorch(int _x, int _y);

	void setEmpty(int _x, int _y);

	//void addWall(int _x, int _y);
	//void addSand(int _x, int _y);
	//void addWater(int _x, int _y);
	//void addFire(int _x, int _y);
	//void addSmoke(int _x, int _y);
	//void addMetal(int _x, int _y);
	//void addSteam(int _x, int _y);

	void brush();

	void wall(int _x, int _y);
	void sand(int _x, int _y);
	void water(int _x, int _y);
	void fire(int _x, int _y);
	void smoke(int _x, int _y);
	void metal(int _x, int _y);
	void steam(int _x, int _y);
	void torch(int _x, int _y);

	std::uint8_t m_updateLatch;
	std::vector<Particle> m_particles;
	std::mt19937 m_rg;
	std::uniform_int_distribution<int> m_dist;
	std::uniform_int_distribution<int> m_dWall;
	std::uniform_int_distribution<int> m_dSand;
	std::uniform_int_distribution<int> m_dWater;
	std::uniform_int_distribution<int> m_dFire;

	int m_brushSize;
	int m_mousex;
	int m_mousey;
	int m_particleChoice;

	//
	// INLINE FUNCTIONS
	// functions defined inside class definition are implicitly inline
	void flipUpdateLatch() { m_updateLatch = ~m_updateLatch; }
	void flipLatch(int _x, int _y) { 
		m_particles[getIndex(_x, _y)].m_latch = m_updateLatch;
	}

	size_t getIndex(int _x, int _y) {
		return size_t(TEXTURE_COLS * _y + _x);
	}

	bool opposites(size_t _idx1, size_t _idx2) {
		return (m_particles[_idx1].m_latch && m_particles[_idx2].m_latch)
			|| (!m_particles[_idx1].m_latch && !m_particles[_idx2].m_latch);
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

	void pSwap(size_t _idx1, size_t _idx2) {
		Particle tmp = m_particles[_idx1];

		m_particles[_idx1].m_id = m_particles[_idx2].m_id;
		m_particles[_idx1].m_latch = m_updateLatch;
		m_particles[_idx1].m_color.x = m_particles[_idx2].m_color.x;
		m_particles[_idx1].m_color.y = m_particles[_idx2].m_color.y;
		m_particles[_idx1].m_color.z = m_particles[_idx2].m_color.z;
		m_particles[_idx1].m_temp = m_particles[_idx2].m_temp;
		m_particles[_idx1].m_specHeat = m_particles[_idx2].m_specHeat;

		m_particles[_idx2].m_id = tmp.m_id;
		m_particles[_idx2].m_latch = m_updateLatch;
		m_particles[_idx2].m_color.x = tmp.m_color.x;
		m_particles[_idx2].m_color.y = tmp.m_color.y;
		m_particles[_idx2].m_color.z = tmp.m_color.z;
		m_particles[_idx2].m_temp = tmp.m_temp;
		m_particles[_idx2].m_specHeat = tmp.m_specHeat;
	}

	void swapEmpty(size_t _idx1, size_t _idx2) {
		uint16_t temp = m_particles[_idx1].m_temp;

		m_particles[_idx1].m_id = m_particles[_idx2].m_id;
		m_particles[_idx1].m_color.x = m_particles[_idx2].m_color.x;
		m_particles[_idx1].m_color.y = m_particles[_idx2].m_color.y;
		m_particles[_idx1].m_color.z = m_particles[_idx2].m_color.z;
		m_particles[_idx1].m_temp = m_particles[_idx2].m_temp;
		m_particles[_idx1].m_specHeat = m_particles[_idx2].m_specHeat;

		m_particles[_idx2].m_id = EMPTY;
		m_particles[_idx2].m_color.x = 0;
		m_particles[_idx2].m_color.y = 0;
		m_particles[_idx2].m_color.z = 0;
		m_particles[_idx2].m_temp = temp;
		m_particles[_idx2].m_specHeat = EMPTY_SPECIFICHEAT;
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