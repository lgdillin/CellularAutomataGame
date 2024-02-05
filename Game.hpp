#pragma once

#include <utility>
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
	void update2();
	void commitUpdate();

	//void updateTemp(int _x, int _y);

	void updateSand(int _x, int _y);

	void updateWater2(int _x, int _y);

	void updateFire(int _x, int _y);
	void updateSmoke(int _x, int _y);
	void updateMetal(int _x, int _y);
	void updateSteam(int _x, int _y);
	void updateTorch(int _x, int _y);

	//void setEmpty(int _x, int _y);

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

	std::vector<Particle> m_particles;
	std::vector<std::pair<int, int>> m_changes;

	std::mt19937 m_rg;
	std::uniform_int_distribution<int> m_dist;
	std::uniform_int_distribution<int> m_dWall;
	std::uniform_int_distribution<int> m_dSand;
	std::uniform_int_distribution<int> m_dWater;
	std::uniform_int_distribution<int> m_dFire;

	int m_brushSize;
	int m_mousex;
	int m_mousey;
	int m_selectedParticle;

	//
	// INLINE FUNCTIONS
	// functions defined inside class definition are implicitly inline
	void setParticle(int _x, int _y) {

	}

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

	void storeChange(int _x1, int _y1, int _x2, int _y2) {
		m_changes.emplace_back(getIndex(_x1, _y1), getIndex(_x2, _y2));
	}

	void cTemps(int _x, int _y) {
		uint16_t td = m_particles[getIndex(_x, _y)].m_tdiff;
		//uint16_t td2 = m_particles[_idx2].m_tdiff;

		uint16_t center = inBounds(_x, _y) ? m_particles[getIndex(_x, _y)].m_temp : EMPTY_BASETEMP;
		uint16_t left = inBounds(_x - 1, _y) ? m_particles[getIndex(_x - 1, _y)].m_temp : EMPTY_BASETEMP;
		uint16_t right = inBounds(_x + 1, _y) ? m_particles[getIndex(_x + 1, _y)].m_temp : EMPTY_BASETEMP;
		uint16_t bottom = inBounds(_x, _y - 1) ? m_particles[getIndex(_x, _y - 1)].m_temp : EMPTY_BASETEMP;
		uint16_t top = inBounds(_x, _y + 1) ? m_particles[getIndex(_x, _y + 1)].m_temp : EMPTY_BASETEMP;
		uint16_t topLeft = inBounds(_x - 1, _y + 1) ? m_particles[getIndex(_x - 1, _y + 1)].m_temp : EMPTY_BASETEMP;
		uint16_t topRight = inBounds(_x + 1, _y + 1) ? m_particles[getIndex(_x + 1, _y + 1)].m_temp : EMPTY_BASETEMP;
		uint16_t bottomLeft = inBounds(_x - 1, _y - 1) ? m_particles[getIndex(_x - 1, _y - 1)].m_temp : EMPTY_BASETEMP;
		uint16_t bottomRight = inBounds(_x + 1, _y - 1) ? m_particles[getIndex(_x + 1, _y - 1)].m_temp : EMPTY_BASETEMP;


		//(*m_bPtr)[getIndex(_x, _y)].m_temp = static_cast<uint16_t>(
		//	center + (td / (float)1000) * (right - 2 * center + left + top - 2 * center + bottom));
		float tdf = (float)td / 10000;
		m_particles[getIndex(_x, _y)].m_temp = static_cast<uint16_t>(
			center
			+ tdf * (right - 2 * center + left)
			+ tdf * (top - 2 * center + bottom)
			+ 0.5f * tdf * (topRight - 2 * center + bottomLeft)
			+ 0.5f * tdf * (topLeft - 2 * center + bottomRight)
			);
	}

	void setEmpty(int _x, int _y) {
		m_particles[getIndex(_x, _y)].m_id = EMPTY;
		m_particles[getIndex(_x, _y)].m_tdiff = EMPTY_TDIFF;
		m_particles[getIndex(_x, _y)].m_temp = EMPTY_BASETEMP;
		m_particles[getIndex(_x, _y)].m_color.x = 0;
		m_particles[getIndex(_x, _y)].m_color.y = 0;
		m_particles[getIndex(_x, _y)].m_color.z = 0;
	}

	void pSwap(size_t _idx1, size_t _idx2) {
		m_particles[_idx1].m_id = m_particles[_idx2].m_id;
		m_particles[_idx1].m_color.x = m_particles[_idx2].m_color.x;
		m_particles[_idx1].m_color.y = m_particles[_idx2].m_color.y;
		m_particles[_idx1].m_color.z = m_particles[_idx2].m_color.z;
		m_particles[_idx1].m_temp = m_particles[_idx2].m_temp;
		m_particles[_idx1].m_tdiff = m_particles[_idx2].m_tdiff;
		
		m_particles[_idx2].m_id = m_particles[_idx1].m_id;
		m_particles[_idx2].m_color.x = m_particles[_idx1].m_color.x;
		m_particles[_idx2].m_color.y = m_particles[_idx1].m_color.y;
		m_particles[_idx2].m_color.z = m_particles[_idx1].m_color.z;
		m_particles[_idx2].m_temp = m_particles[_idx1].m_temp;
		m_particles[_idx2].m_tdiff = m_particles[_idx1].m_tdiff;
	}

private:
};