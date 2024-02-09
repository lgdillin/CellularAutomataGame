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
	void updateTemps();

	//void updateTemp(int _x, int _y);

	void updateEmpty(int _x, int _y);

	void updateSand(int _x, int _y);

	void updateWater(int _x, int _y);

	void updateFire(int _x, int _y);
	void updateSmoke(int _x, int _y);
	void updateMetal(int _x, int _y);
	void updateSteam(int _x, int _y);
	void updateTorch(int _x, int _y);

	//void calcTemp(int _x, int _y);

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

	int dummy = 0;
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

	uint16_t boundaryTemp(int _xb, int _yb, int _x, int _y) {
		switch (inBounds(_xb, _yb)) {
		case true:
			return m_particles[getIndex(_xb, _yb)].m_temp;
			break;
		case false:
			return m_particles[getIndex(_x, _y)].m_temp;;
			break;
		}
	}

	uint16_t getTemp(int _x, int _y) {
		switch (inBounds(_x, _y)) {
		case true:
			return m_particles[getIndex(_x, _y)].m_temp;
			break;
		case false:
			return 0;
			break;
		}
	}

	uint16_t getTemp2(int _col, int _row) {
		return m_particles[_row * TEXTURE_ROWS + _col].m_temp;
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
	
	void extinguishFire(int _x, int _y) {
		m_particles[getIndex(_x, _y)].m_id = EMPTY;
		m_particles[getIndex(_x, _y)].m_temp = getTemp(_x, _y);
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

	//void calcTemp(int _x, int _y) {
	//	float diff = T_CON(getId(_x, _y)) / (DENSITY(getId(_x, _y)) * S_HEAT(getId(_x, _y)));
	//	float c = getTemp(_x, _y);
	//	float t = getTemp(_x, _y + 1);
	//	float l = getTemp(_x - 1, _y);
	//	float r = getTemp(_x + 1, _y);
	//	float b = getTemp(_x, _y - 1);
	//	float tn1 = c + diff * (r + l + t + b - 4 * c);
	//	//m_tempGrid[getIndex(_x, _y)] = static_cast<uint16_t>(tn1);
	//}


private:
};