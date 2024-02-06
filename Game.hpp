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

	void update2();
	void commitUpdate();

	//void updateTemp(int _x, int _y);

	void updateSand(int _x, int _y);

	void updateWater(int _x, int _y);

	void updateFire(int _x, int _y);
	void updateSmoke(int _x, int _y);
	void updateMetal(int _x, int _y);
	void updateSteam(int _x, int _y);
	void updateTorch(int _x, int _y);

	void calcTemp(int _x, int _y);

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
	std::vector<uint16_t> m_tempGrid;

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
		return inBounds(_x, _y) ? m_particles[getIndex(_x, _y)].m_id : 255;
	}

	uint16_t getTemp(int _x, int _y) {
		return inBounds(_x, _y) ? m_particles[getIndex(_x, _y)].m_temp : EMPTY_BASETEMP;
	}

	void storeChange(int _x1, int _y1, int _x2, int _y2) {
		m_changes.emplace_back(getIndex(_x1, _y1), getIndex(_x2, _y2));
	}

	void setEmpty(int _x, int _y) {
		m_particles[getIndex(_x, _y)].m_id = EMPTY;
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
		
		m_particles[_idx2].m_id = m_particles[_idx1].m_id;
		m_particles[_idx2].m_color.x = m_particles[_idx1].m_color.x;
		m_particles[_idx2].m_color.y = m_particles[_idx1].m_color.y;
		m_particles[_idx2].m_color.z = m_particles[_idx1].m_color.z;
		m_particles[_idx2].m_temp = m_particles[_idx1].m_temp;
	}

	float T_CON(uint8_t _id) {
		switch (_id) {
		case EMPTY:
			return 0.0257f;
			break;
		case WALL:
			return 1;
			break;
		case SAND:
			return 3;
			break;
		case WATER:
			return 0.606f;
			break;
		case FIRE:
			return 0.06f;
			break;
		case SMOKE:
			return 1;
			break;
		case METAL:
			return 80.0f;
			break;
		case STEAM:
			return 1;
			break;
		case TORCH:
			return 0.06f;
			break;
		case OOB:
			return 0.0257;
			break;
		}
	}

	float S_HEAT(uint8_t _id) {
		switch (_id) {
		case EMPTY:
			return 1005.0f;
			break;
		case WALL:
			return 1;
			break;
		case SAND:
			return 75;
			break;
		case WATER:
			return 4186.0f;
			break;
		case FIRE:
			return 1003.0f;
			break;
		case SMOKE:
			return 1;
			break;
		case METAL:
			return 449.0f;
			break;
		case STEAM:
			return 1;
			break;
		case TORCH:
			return 1005.0f;
			break;
		case OOB:
			return 1005.0f;
			break;
		}
	}

	float DENSITY(int _id) {
		switch (_id) {
		case EMPTY:
			return 1.225f;
			break;
		case WALL:
			return 1;
			break;
		case SAND:
			return 75;
			break;
		case WATER:
			return 1000.0f;
			break;
		case FIRE:
			return 1.1f;
			break;
		case SMOKE:
			return 1;
			break;
		case METAL:
			return 7870.0f;
			break;
		case STEAM:
			return 1;
			break;
		case TORCH:
			return 1.05f;
			break;
		case OOB:
			return 1.005f;
			break;
		}
	}

private:
};