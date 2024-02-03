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

	//void updateTemp(int _x, int _y);

	void updateSand(int _x, int _y);
	void updateWater(int _x, int _y);
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

	std::vector<Particle> *m_wPtr;
	std::vector<Particle> *m_rPtr;
	std::pair<std::vector<Particle>, std::vector<Particle>> m_pBuffers;
	uint8_t m_bufferWriteMode;
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

	void swapBuffers() {
		switch (m_bufferWriteMode) {
		case 1:
			m_bufferWriteMode = 2;
			m_wPtr = &m_pBuffers.second;
			m_rPtr = &m_pBuffers.first;
			break;
		case 2:
			m_bufferWriteMode = 1;
			m_wPtr = &m_pBuffers.first;
			m_rPtr = &m_pBuffers.second;
			break;
		}
		flushWriteBuffer();
	}

	void flushWriteBuffer() {
		for (int i = 0; i < TEXTURE_COLS; ++i) {
			for (int j = 0; j < TEXTURE_ROWS; ++j) {
				(*m_wPtr)[getIndex(i, j)].m_updated = 0;
				setEmpty(i, j);
			}
		}
	}

	std::vector<Particle> *getPresentBuffer() {
		return (m_bufferWriteMode == 1 ? &m_pBuffers.second : &m_pBuffers.first);
	}

	std::vector<Particle> *getWriteBuffer() {
		return m_bufferWriteMode == 1 ? &m_pBuffers.first : &m_pBuffers.second;
	}

	size_t getIndex(int _x, int _y) {
		return size_t(TEXTURE_COLS * _y + _x);
	}

	bool inBounds(int _x, int _y) {
		return (_x >= 0 && _x < TEXTURE_COLS) && (_y >= 0 && _y < TEXTURE_ROWS);
	}

	bool isEmpty(int _x, int _y) {
		return inBounds(_x, _y) && (*m_rPtr)[getIndex(_x, _y)].m_id == EMPTY;
	}

	bool isWater(int _x, int _y) {
		return inBounds(_x, _y) && (*m_rPtr)[getIndex(_x, _y)].m_id == WATER;
	}

	bool isSmoke(int _x, int _y) {
		return inBounds(_x, _y) && (*m_rPtr)[getIndex(_x, _y)].m_id == SMOKE;
	}

	uint8_t getId(int _x, int _y) {
		return (*m_rPtr)[getIndex(_x, _y)].m_id;
	}

	void noSwapUpdate(size_t _idx) {
		(*m_wPtr)[_idx].m_id = (*m_rPtr)[_idx].m_id;
		(*m_wPtr)[_idx].m_latch = true;
		(*m_wPtr)[_idx].m_color.x = (*m_rPtr)[_idx].m_color.x;
		(*m_wPtr)[_idx].m_color.y = (*m_rPtr)[_idx].m_color.y;
		(*m_wPtr)[_idx].m_color.z = (*m_rPtr)[_idx].m_color.z;
		(*m_wPtr)[_idx].m_temp = (*m_rPtr)[_idx].m_temp;
		(*m_wPtr)[_idx].m_tdiff = (*m_rPtr)[_idx].m_tdiff;
	}

	void cTemps(int _x, int _y) {
		uint16_t td = (*m_rPtr)[getIndex(_x, _y)].m_tdiff;
		//uint16_t td2 = m_particles[_idx2].m_tdiff;

		uint16_t center = inBounds(_x, _y) ? (*m_rPtr)[getIndex(_x, _y)].m_temp : EMPTY_BASETEMP;
		uint16_t left = inBounds(_x - 1, _y) ? (*m_rPtr)[getIndex(_x - 1, _y)].m_temp : EMPTY_BASETEMP;
		uint16_t right = inBounds(_x + 1, _y) ? (*m_rPtr)[getIndex(_x + 1, _y)].m_temp : EMPTY_BASETEMP;
		uint16_t bottom = inBounds(_x, _y - 1) ? (*m_rPtr)[getIndex(_x, _y - 1)].m_temp : EMPTY_BASETEMP;
		uint16_t top = inBounds(_x, _y + 1) ? (*m_rPtr)[getIndex(_x, _y + 1)].m_temp : EMPTY_BASETEMP;
		uint16_t topLeft = inBounds(_x - 1, _y + 1) ? (*m_rPtr)[getIndex(_x - 1, _y + 1)].m_temp : EMPTY_BASETEMP;
		uint16_t topRight = inBounds(_x + 1, _y + 1) ? (*m_rPtr)[getIndex(_x + 1, _y + 1)].m_temp : EMPTY_BASETEMP;
		uint16_t bottomLeft = inBounds(_x - 1, _y - 1) ? (*m_rPtr)[getIndex(_x - 1, _y - 1)].m_temp : EMPTY_BASETEMP;
		uint16_t bottomRight = inBounds(_x + 1, _y - 1) ? (*m_rPtr)[getIndex(_x + 1, _y - 1)].m_temp : EMPTY_BASETEMP;


		//(*m_bPtr)[getIndex(_x, _y)].m_temp = static_cast<uint16_t>(
		//	center + (td / (float)1000) * (right - 2 * center + left + top - 2 * center + bottom));
		float tdf = (float)td / 10000;
		(*m_rPtr)[getIndex(_x, _y)].m_temp = static_cast<uint16_t>(
			center
			+ tdf * (right - 2 * center + left)
			+ tdf * (top - 2 * center + bottom)
			+ 0.5f * tdf * (topRight - 2 * center + bottomLeft)
			+ 0.5f * tdf * (topLeft - 2 * center + bottomRight)
			);
	}

	void setEmpty(int _x, int _y) {
		(*m_wPtr)[getIndex(_x, _y)].m_id = EMPTY;
		(*m_wPtr)[getIndex(_x, _y)].m_tdiff = EMPTY_TDIFF;
		(*m_wPtr)[getIndex(_x, _y)].m_temp = EMPTY_BASETEMP;
		(*m_wPtr)[getIndex(_x, _y)].m_color.x = 0;
		(*m_wPtr)[getIndex(_x, _y)].m_color.y = 0;
		(*m_wPtr)[getIndex(_x, _y)].m_color.z = 0;
	}

	void pSwap(size_t _idx1, size_t _idx2) {
		//if ((*m_wPtr)[_idx1].m_id == EMPTY) {
			(*m_wPtr)[_idx1].m_id = (*m_rPtr)[_idx2].m_id;
			(*m_wPtr)[_idx1].m_latch = true;
			(*m_wPtr)[_idx1].m_color.x = (*m_rPtr)[_idx2].m_color.x;
			(*m_wPtr)[_idx1].m_color.y = (*m_rPtr)[_idx2].m_color.y;
			(*m_wPtr)[_idx1].m_color.z = (*m_rPtr)[_idx2].m_color.z;
			(*m_wPtr)[_idx1].m_temp = (*m_rPtr)[_idx2].m_temp;
			(*m_wPtr)[_idx1].m_tdiff = (*m_rPtr)[_idx2].m_tdiff;
		//}

		//if ((*m_wPtr)[_idx2].m_id == EMPTY) {
			(*m_wPtr)[_idx2].m_id = (*m_rPtr)[_idx1].m_id;
			(*m_wPtr)[_idx2].m_latch = true;
			(*m_wPtr)[_idx2].m_color.x = (*m_rPtr)[_idx1].m_color.x;
			(*m_wPtr)[_idx2].m_color.y = (*m_rPtr)[_idx1].m_color.y;
			(*m_wPtr)[_idx2].m_color.z = (*m_rPtr)[_idx1].m_color.z;
			(*m_wPtr)[_idx2].m_temp = (*m_rPtr)[_idx1].m_temp;
			(*m_wPtr)[_idx2].m_tdiff = (*m_rPtr)[_idx1].m_tdiff;
		//}
	}

	void swapEmpty(size_t _idx1, size_t _idx2) {
		(*m_wPtr)[_idx1].m_id = (*m_rPtr)[_idx2].m_id;
		(*m_wPtr)[_idx1].m_color.x = (*m_rPtr)[_idx2].m_color.x;
		(*m_wPtr)[_idx1].m_color.y = (*m_rPtr)[_idx2].m_color.y;
		(*m_wPtr)[_idx1].m_color.z = (*m_rPtr)[_idx2].m_color.z;
		(*m_wPtr)[_idx1].m_temp = (*m_rPtr)[_idx2].m_temp;
		(*m_wPtr)[_idx1].m_tdiff = (*m_rPtr)[_idx2].m_tdiff;
		  
		(*m_wPtr)[_idx2].m_id = (*m_rPtr)[_idx1].m_id;
		(*m_wPtr)[_idx2].m_color.x = (*m_rPtr)[_idx1].m_color.x;
		(*m_wPtr)[_idx2].m_color.y = (*m_rPtr)[_idx1].m_color.y;
		(*m_wPtr)[_idx2].m_color.z = (*m_rPtr)[_idx1].m_color.z;
		(*m_wPtr)[_idx2].m_temp = (*m_rPtr)[_idx1].m_temp;
		(*m_wPtr)[_idx2].m_tdiff = (*m_rPtr)[_idx1].m_tdiff;
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