#include "Game.hpp"

Game::Game() {
	m_changes.reserve(1000);

	std::random_device rd;
	m_rg = std::mt19937(rd());
	m_dWall = std::uniform_int_distribution<int>(40, 50);
	m_dSand = std::uniform_int_distribution<int>(120, 150);
	m_dWater = std::uniform_int_distribution<int>(120, 150);
	m_dFire = std::uniform_int_distribution<int>(20, 40);
	m_dist = std::uniform_int_distribution<int>(0, 99);

	m_tempGrid = std::vector<uint16_t>(TEXTURE_ROWS * TEXTURE_COLS);
	m_particles = std::vector<Particle>(TEXTURE_ROWS * TEXTURE_COLS);
	m_selectedParticle = 0;

	m_mousex = 0;
	m_mousey = 0;
	m_brushSize = 1;
}

Game::~Game() {
}

void Game::initialize() {

}

void Game::update2() {
	for (int i = 0; i < TEXTURE_COLS; ++i) {
		for (int j = 0; j < TEXTURE_ROWS; ++j) {
			switch (m_particles[j * TEXTURE_ROWS + i].m_id) {
			case EMPTY:
				break;
			case WALL:
				break;
			case SAND:
				updateSand(i, j);
				break;
			case WATER:
				updateWater(i, j);
				break;
			case FIRE:
				updateFire(i, j);
				break;
			case METAL:
				updateMetal(i, j);
				break;

			case TORCH:
				updateTorch(i, j);
				break;
			}

			calcTemp(i, j);
		}
	}

	commitUpdate();
}

void Game::commitUpdate() {
	//for (size_t i = 0; i < m_changes.size(); ++i) {
	//	if (m_particles[m_changes[i].first].m_id != EMPTY) {
	//		m_changes[i] = m_changes.back(); m_changes.pop_back();
	//		--i;
	//	}
	//}

	std::sort(m_changes.begin(), m_changes.end(),
		[](auto &a, auto &b) { return a.first < b.first; }
	);

	m_changes.emplace_back(-1, -1);
	size_t iprev = 0;
	for (size_t i = 0; i < m_changes.size() - 1; i++) {

		// since we have sorted our moves by the src, we need to skip along
		// until we find where the next src starts in the vector
		if (m_changes[i + 1].first != m_changes[i].first) {
			// pick a value between iprev and i - prev
			size_t rand = iprev + mymath::randInt(i - iprev);

			size_t dst = m_changes[rand].first;
			size_t src = m_changes[rand].second;

			Particle tmp = m_particles[dst];
			m_particles[dst] = m_particles[src];
			m_particles[src] = tmp;

			iprev = i + 1;
		}
	}
	m_changes.clear();
	
	// update temps
	for (int i = 0; i < m_particles.size(); ++i) {
		m_particles[i].m_temp = m_tempGrid[i];
	}
}

void Game::updateSand(int _x, int _y) {
	bool emptyBottom = isEmpty(_x, _y - 1);
	bool emptyBottomLeft = isEmpty(_x - 1, _y - 1);
	bool emptyBottomRight = isEmpty(_x + 1, _y - 1);
	bool waterBottom = isWater(_x, _y - 1);
	bool waterBottomLeft = isWater(_x - 1, _y - 1);
	bool waterBottomRight = isWater(_x + 1, _y - 1);
	bool waterLeft = isWater(_x, _y - 1);
	bool waterRight = isWater(_x, _y + 1);

	int choice = m_dist(m_rg) % 3;
	choice = m_dist(m_rg) % 3;
	switch (choice) {
	case 0:
		if (emptyBottom) {
			storeChange(_x, _y, _x, _y - 1);
			return;
		} break;
	case 1:
		if (emptyBottomLeft) {
			storeChange(_x, _y, _x - 1, _y - 1);
			return;
		} break;
	case 2:
		if (emptyBottomRight) {
			storeChange(_x, _y, _x + 1, _y - 1);
			return;
		} break;
	}

	choice = m_dist(m_rg) % 5;
	switch (choice) {
	case 0:
		if (waterBottom) {
			storeChange(_x, _y, _x, _y - 1);
			return;
		} break;
	case 1:
		if (waterBottomLeft) {
			storeChange(_x, _y, _x - 1, _y - 1);
			return;
		} break;
	case 2:
		if (waterBottomRight) {
			storeChange(_x, _y, _x + 1, _y - 1);
			return;
		} break;
	case 3:
		if (waterLeft) {
			storeChange(_x, _y, _x - 1, _y);
			return;
		} break;
	case 4:
		if (waterRight) {
			storeChange(_x, _y, _x + 1, _y);
			return;
		} break;
	}
}

void Game::updateWater(int _x, int _y) {
	float t = m_particles[getIndex(_x, _y)].m_temp / 1000;
	m_particles[getIndex(_x, _y)].m_color.x = mymath::lerp(20, 150, t);
	m_particles[getIndex(_x, _y)].m_color.y = mymath::lerp(20, 150, t);
	m_particles[getIndex(_x, _y)].m_color.z = mymath::lerp(130, 150, t);

	bool emptyBottom = isEmpty(_x, _y - 1);
	bool emptyBottomLeft = isEmpty(_x - 1, _y - 1);
	bool emptyBottomRight = isEmpty(_x + 1, _y - 1);
	bool emptyLeft = isEmpty(_x - 1, _y);
	bool emptyRight = isEmpty(_x + 1, _y);

	if (emptyBottom) {
		storeChange(_x, _y, _x, _y - 1);
		return;
	}

	int choice = rand() % 2;
	switch (choice) {
	case 0:
		if (emptyBottomLeft) {
			storeChange(_x, _y, _x - 1, _y - 1);
			return;
		} else if (emptyLeft) {
			storeChange(_x, _y, _x - 1, _y);
			return;
		} break;
	case 1:
		if (emptyBottomRight) {
			storeChange(_x, _y, _x + 1, _y - 1);
			return;
		} else if (emptyRight) {
			storeChange(_x, _y, _x + 1, _y);
			return;
		} break;
	}
}


void Game::updateFire(int _x, int _y) {
	float t = (m_particles[getIndex(_x, _y)].m_temp - FIRE_MINTEMP) / (FIRE_BASETEMP - FIRE_MINTEMP);
	m_particles[getIndex(_x, _y)].m_color.x = mymath::lerp(240, 180, t);
	m_particles[getIndex(_x, _y)].m_color.y = mymath::lerp(50, 130, t);
	m_particles[getIndex(_x, _y)].m_color.z = mymath::lerp(20, 20, t);

	if (m_particles[getIndex(_x, _y)].m_temp <= FIRE_MINTEMP) {
		setEmpty(_x, _y);
		return;
	} 

	bool emptyTop = isEmpty(_x, _y + 1);
	bool emptyTopLeft = isEmpty(_x - 1, _y + 1);
	bool emptyTopRight = isEmpty(_x + 1, _y + 1);
	bool emptyLeft = isEmpty(_x - 1, _y);
	bool emptyRight = isEmpty(_x + 1, _y);
	bool emptyBottomLeft = isEmpty(_x - 1, _y - 1);
	bool emptyBottomRight = isEmpty(_x + 1, _y - 1);

	int choice = m_dist(m_rg) % 3;
	switch (choice) {
	case 0:
		if (emptyTopLeft)
			storeChange(_x, _y, _x - 1, _y + 1);
		else if (emptyLeft)
			storeChange(_x, _y, _x - 1, _y);
		break;
	case 1:
		if (emptyTopRight)
			storeChange(_x, _y, _x + 1, _y + 1);
		else if (emptyRight)
			storeChange(_x, _y, _x + 1, _y);
		break;
	case 2:
		if (emptyTop) {
			storeChange(_x, _y, _x, _y + 1);
			return;
		}

		choice = m_dist(m_rg) % 2;
		switch (choice) {
		case 0:
			if(emptyBottomLeft)
				storeChange(_x, _y, _x - 1, _y - 1); break;
		case 1:
			if(emptyBottomRight)
				storeChange(_x, _y, _x + 1, _y - 1); break;
		}
		break;
	}
}

void Game::updateSmoke(int _x, int _y) {
	if (m_particles[getIndex(_x, _y)].m_temp < SMOKE_MINTEMP) {
		setEmpty(_x, _y);
		return;
	} else if (m_particles[getIndex(_x, _y)].m_temp > FIRE_MINTEMP) {
		fire(_x, _y);
	} else {

	}

	bool emptyTop = isEmpty(_x, _y + 1);
	bool emptyTopLeft = isEmpty(_x - 1, _y + 1);
	bool emptyTopRight = isEmpty(_x + 1, _y + 1);
	bool emptyLeft = isEmpty(_x - 1, _y);
	bool emptyRight = isEmpty(_x + 1, _y);

	if (emptyTop) {
		pSwap(getIndex(_x, _y), getIndex(_x, _y + 1));
		return;
	}

	int choice = rand() % 3;
	switch (choice) {
	case 0:
		if (emptyLeft)
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y));
		else if (emptyTopLeft)
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y + 1));
		break;
	case 1:
		if (emptyRight)
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y));
		else if (emptyTopRight)
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y + 1));
		break;
	default:
		break;
	}
}

void Game::updateMetal(int _x, int _y) {
	float t = (m_particles[getIndex(_x, _y)].m_temp) / 200;

	m_particles[getIndex(_x, _y)].m_color.x = mymath::lerp(70, 150, t);
	m_particles[getIndex(_x, _y)].m_color.y = mymath::lerp(70, 110, t);
	m_particles[getIndex(_x, _y)].m_color.z = mymath::lerp(70, 110, t);

	//if (temp > 100) {

		//m_particles[getIndex(_x, _y)].m_color.y = mymath::lerpU8(
		//	temp, 0, 250, METAL_COLOR[0], 255);
		//m_particles[getIndex(_x, _y)].m_color.z = mymath::lerpU8(
		//	temp, 0, 250, METAL_COLOR[0], 255);
	//}

	//(*m_bPtr)[getIndex(_x, _y)].m_color.x = mymath::clampu8(temp);
	

	//if (temp < 100) {
	//	m_particles[getIndex(_x, _y)].m_color.x = METAL_COLOR[0] + temp;
	//} else {
	//	m_particles[getIndex(_x, _y)].m_color.x = METAL_COLOR[0] + temp;
	//	m_particles[getIndex(_x, _y)].m_color.y = METAL_COLOR[1] + temp;
	//	m_particles[getIndex(_x, _y)].m_color.z = METAL_COLOR[2] + temp;
	//}

}

void Game::updateSteam(int _x, int _y) {
	if (m_particles[getIndex(_x, _y)].m_temp < WATER_BASETEMP) {
		water(_x, _y);
		return;
	}

	bool emptyTop = isEmpty(_x, _y + 1);
	bool emptyTopLeft = isEmpty(_x - 1, _y + 1);
	bool emptyTopRight = isEmpty(_x + 1, _y + 1);
	bool emptyLeft = isEmpty(_x - 1, _y);
	bool emptyRight = isEmpty(_x + 1, _y);
	bool emptyBottomLeft = isEmpty(_x - 1, _y - 1);
	bool emptyBottomRight = isEmpty(_x + 1, _y - 1);

	int choice = m_dist(m_rg) % 3;
	switch (choice) {
	case 0:
		if (emptyTopLeft) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y + 1));
			return;
		} else if (emptyLeft) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y));
			return;
		}
		break;
	case 1:
		if (emptyTopRight) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y + 1));
			return;
		} else if (emptyRight) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y));
			return;
		}
		break;
	default:
		if (emptyTop) {
			pSwap(getIndex(_x, _y), getIndex(_x, _y + 1));
			return;
		}

		choice = m_dist(m_rg) % 2;
		switch (choice) {
		case 0:
			if (emptyBottomLeft) {
				pSwap(getIndex(_x, _y), getIndex(_x - 1, _y - 1));
				return;
			}
			break;
		case 1:
			if (emptyBottomRight) {
				pSwap(getIndex(_x, _y), getIndex(_x + 1, _y - 1));
				return;
			}
			break;
		}
		break;
	}

	bool waterLeft = isWater(_x, _y - 1);
	bool waterRight = isWater(_x, _y + 1);
	bool waterTop = isWater(_x, _y + 1);
	bool waterTopLeft = isWater(_x - 1, _y + 1);
	bool waterTopRight = isWater(_x + 1, _y + 1);
	bool waterBottomLeft = isWater(_x - 1, _y - 1);
	bool waterBottomRight = isWater(_x + 1, _y - 1);
	choice = m_dist(m_rg) % 3;
	switch (choice) {
	case 0:
		if (waterTopLeft)
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y + 1));
		else if (waterLeft)
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y));
		break;
	case 1:
		if (waterTopRight)
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y + 1));
		else if (waterRight)
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y));
		break;
	default:
		if (waterTop) {
			pSwap(getIndex(_x, _y), getIndex(_x, _y + 1));
			return;
		}

		choice = m_dist(m_rg) % 2;
		switch (choice) {
		case 0:
			if (waterBottomLeft)
				pSwap(getIndex(_x, _y), getIndex(_x - 1, _y - 1)); break;
		case 1:
			if (waterBottomRight)
				pSwap(getIndex(_x, _y), getIndex(_x + 1, _y - 1)); break;
		}
		break;
	}
}

void Game::updateTorch(int _x, int _y) {
	if (isEmpty(_x - 1, _y + 1))
		fire(_x - 1, _y + 1);
	if (isEmpty(_x, _y + 1))
		fire(_x, _y + 1);
	if (isEmpty(_x + 1, _y + 1))
		fire(_x + 1, _y + 1);
	if (isEmpty(_x - 1, _y))
		fire(_x - 1, _y);
	if (isEmpty(_x + 1, _y))
		fire(_x + 1, _y);
	if (isEmpty(_x - 1, _y - 1))
		fire(_x - 1, _y - 1);
	if (isEmpty(_x, _y - 1))
		fire(_x, _y - 1);
	if (isEmpty(_x + 1, _y - 1))
		fire(_x + 1, _y - 1);
}

void Game::calcTemp(int _x, int _y) {
		float c = getTemp(_x, _y);
	float t = getTemp(_x, _y + 1);
	float l = getTemp(_x - 1, _y);
	float r = getTemp(_x + 1, _y);
	float b = getTemp(_x, _y - 1);

	float diff = T_CON(getId(_x, _y)) / (DENSITY(getId(_x, _y)) * S_HEAT(getId(_x, _y)));
	float tn1 = getTemp(_x, _y) + diff * (r + l - 4 * c + t + b);
	m_tempGrid[getIndex(_x, _y)] = static_cast<uint16_t>(tn1);

	if (getId(_x, _y) == METAL) {
		std::cout << "METAL " << tn1 << std::endl;
	}

	if (getId(_x, _y) == WATER) {
		std::cout << "WATER " << tn1 << std::endl;
	}
}

void Game::brush() {
	int scalex = static_cast<int>((m_mousex / (float)SCREEN_WIDTH) * TEXTURE_COLS);
	// SDL mouse input has y increase at the bottom of window, so flip
	int scaley = static_cast<int>((m_mousey / (float)SCREEN_HEIGHT) * TEXTURE_ROWS);
	scaley = (TEXTURE_ROWS) - scaley;

	scalex = scalex >= TEXTURE_COLS ? TEXTURE_COLS - 1 : scalex;
	int x = scalex < 0 ? 0 : scalex;
	scaley = scaley >= TEXTURE_ROWS ? TEXTURE_ROWS - 1 : scaley;
	int y = scaley < 0 ? 0 : scaley;

	int halfBrush = (int)(m_brushSize / 2);
	for (int i = -halfBrush; i <= halfBrush; ++i) {
		for (int j = -halfBrush; j <= halfBrush; ++j) {
			if (!inBounds(x + i, y + j)) continue;

			switch (m_selectedParticle) {
			case EMPTY:

				break;
			case WALL:
				wall(x + i, y + j);
				break;
			case SAND:
				sand(x + i, y + j);
				break;
			case WATER:
				water(x + i, y + j);
				break;
			case FIRE:
				fire(x + i, y + j);
				break;
			case SMOKE:
				smoke(x + i, y + j);
				break;
			case METAL:
				metal(x + i, y + j);
				break;
			case STEAM:
				steam(x + i, y + j);
				break;
			case TORCH:
				torch(x + i, y + j);
				break;
			default:
				break;
			}
		}
	}
}

void Game::wall(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dWall(m_rg));
	m_particles[getIndex(_x, _y)].m_id = WALL;
	m_particles[getIndex(_x, _y)].m_temp = WALL_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = c;
	m_particles[getIndex(_x, _y)].m_color.y = c;
	m_particles[getIndex(_x, _y)].m_color.z = c;
}

void Game::sand(int _x, int _y) {
	m_particles[getIndex(_x, _y)].m_id = SAND;
	m_particles[getIndex(_x, _y)].m_temp = SAND_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = SAND_COLOR[0];
	m_particles[getIndex(_x, _y)].m_color.y = SAND_COLOR[1];
	m_particles[getIndex(_x, _y)].m_color.z = SAND_COLOR[2];
}

void Game::water(int _x, int _y) {
	m_particles[getIndex(_x, _y)].m_id = WATER;
	m_particles[getIndex(_x, _y)].m_temp = WATER_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = WATER_COLOR[0];
	m_particles[getIndex(_x, _y)].m_color.y = WATER_COLOR[1];
	m_particles[getIndex(_x, _y)].m_color.z = WATER_COLOR[2];
}

void Game::fire(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dFire(m_rg));
	m_particles[getIndex(_x, _y)].m_id = FIRE;
	m_particles[getIndex(_x, _y)].m_temp = FIRE_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = FIRE_COLOR[0];
	m_particles[getIndex(_x, _y)].m_color.y = FIRE_COLOR[1];
	m_particles[getIndex(_x, _y)].m_color.z = FIRE_COLOR[2];
}

void Game::smoke(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dWall(m_rg));
	m_particles[getIndex(_x, _y)].m_id = SMOKE;
	m_particles[getIndex(_x, _y)].m_temp = SMOKE_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = SMOKE_COLOR[0];
	m_particles[getIndex(_x, _y)].m_color.y = SMOKE_COLOR[1];
	m_particles[getIndex(_x, _y)].m_color.z = SMOKE_COLOR[2];
}

void Game::metal(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dWall(m_rg));
	m_particles[getIndex(_x, _y)].m_id = METAL;
	m_particles[getIndex(_x, _y)].m_temp = METAL_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = METAL_COLOR[0];
	m_particles[getIndex(_x, _y)].m_color.y = METAL_COLOR[1];
	m_particles[getIndex(_x, _y)].m_color.z = METAL_COLOR[2];
}

void Game::steam(int _x, int _y) {
	m_particles[getIndex(_x, _y)].m_id = STEAM;
	m_particles[getIndex(_x, _y)].m_temp = STEAM_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = 100;
	m_particles[getIndex(_x, _y)].m_color.y = 100;
	m_particles[getIndex(_x, _y)].m_color.z = 180;
}

void Game::torch(int _x, int _y) {
	m_particles[getIndex(_x, _y)].m_id = TORCH;
	m_particles[getIndex(_x, _y)].m_temp = TORCH_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = 150;
	m_particles[getIndex(_x, _y)].m_color.y = 150;
	m_particles[getIndex(_x, _y)].m_color.z = 0;
}
