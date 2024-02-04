#include "Game.hpp"

Game::Game() {
	m_bufferWriteMode = 1;
	m_wPtr = &m_pBuffers.first;
	m_rPtr = &m_pBuffers.second;

	std::random_device rd;
	m_rg = std::mt19937(rd());
	m_dWall = std::uniform_int_distribution<int>(40, 50);
	m_dSand = std::uniform_int_distribution<int>(120, 150);
	m_dWater = std::uniform_int_distribution<int>(120, 150);
	m_dFire = std::uniform_int_distribution<int>(20, 40);
	m_dist = std::uniform_int_distribution<int>(0, 99);

	m_pBuffers.first = std::vector<Particle>(TEXTURE_ROWS * TEXTURE_COLS);
	m_pBuffers.second = std::vector<Particle>(TEXTURE_ROWS * TEXTURE_COLS);
	m_particleChoice = 0;

	m_mousex = 0;
	m_mousey = 0;
	m_brushSize = 1;
}

Game::~Game() {
}

void Game::initialize() {

}

void Game::update() {
	for (int i = 0; i < TEXTURE_COLS; ++i) {
		for (int j = 0; j < TEXTURE_ROWS; ++j) {
			//if ((*m_bPtr)[j * TEXTURE_ROWS + i].m_latch) continue;

			switch ((*m_rPtr)[j * TEXTURE_ROWS + i].m_id) {
			case EMPTY:
				
				break;
			case WALL:
				noSwapUpdate(getIndex(i, j));
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
			case SMOKE:
				updateSmoke(i, j);
				break;
			case METAL:
				updateMetal(i, j);
				break;
			case STEAM:
				updateSteam(i, j);
				break;
			case TORCH:
				updateTorch(i, j);
				break;
			}

			// universal
			//cTemps(i, j);
		}
	}

	swapBuffers();
}

void Game::update2() {
	for (int i = 0; i < TEXTURE_COLS; ++i) {
		for (int j = 0; j < TEXTURE_ROWS; ++j) {
			Particle *topLeft = inBounds(i - 1, j + 1) ? &(*m_rPtr)[getIndex(i - 1, j + 1)] : nullptr;
			Particle *top = inBounds(i, j + 1) ? &(*m_rPtr)[getIndex(i, j + 1)] : nullptr;
			Particle *topRight = inBounds(i + 1, j + 1) ? &(*m_rPtr)[getIndex(i + 1, j + 1)] : nullptr;
			Particle *left = inBounds(i - 1, j) ? &(*m_rPtr)[getIndex(i - 1, j)] : nullptr;
			Particle *right = inBounds(i + 1, j) ? &(*m_rPtr)[getIndex(i + 1, j)] : nullptr;
			Particle *bottomLeft = inBounds(i - 1, j - 1) ? &(*m_rPtr)[getIndex(i - 1, j - 1)] : nullptr;
			Particle *bottom = inBounds(i, j - 1) ? &(*m_rPtr)[getIndex(i, j - 1)] : nullptr;
			Particle *bottomRight = inBounds(i + 1, j - 1) ? &(*m_rPtr)[getIndex(i + 1, j - 1)] : nullptr;
		
			Particle *p1 = &(*m_rPtr)[getIndex(i, j)];
			uint8_t candidates = 0;
			for (int a = -1; a <= 1; ++a) {
				for (int b = -1; b <= 1; ++b) {
					Particle *p = inBounds(i + a, j + b) ? &(*m_rPtr)[getIndex(i + a, j + b)] : nullptr;
				
					switch (p->m_id) {
					case EMPTY:
						break;
					case SAND:

					default:
						break;
					}
				}
			}


		}
	}
}

void Game::update3() {
	std::byte zero{ 0b00000000 };

	for (int i = 0; i < TEXTURE_COLS; ++i) {
		for (int j = 0; j < TEXTURE_ROWS; ++j) {
			Particle *destination = &(*m_rPtr)[getIndex(i, j)];
			std::byte candidates{ 0b00000000 };
			std::vector<std::size_t> indices;

			// top left
			Particle *p = getParticle(i - 1, j + 1);
			candidates |= TOP_LEFT & (p != nullptr ? p->m_moves : zero);

			// top
			p = getParticle(i, j + 1);
			candidates |= TOP & (p != nullptr ? p->m_moves : zero);

			// top right
			p = getParticle(i + 1, j + 1);
			candidates |= TOP_RIGHT & (p != nullptr ? p->m_moves : zero);

			// left
			p = getParticle(i - 1, j);
			candidates |= LEFT & (p != nullptr ? p->m_moves : zero);

			// right 
			p = getParticle(i + 1, j);
			candidates |= RIGHT & (p != nullptr ? p->m_moves : zero);

			// bottom left
			p = getParticle(i - 1, j - 1);
			candidates |= BOTTOM_LEFT & (p != nullptr ? p->m_moves : zero);

			// bottom
			p = getParticle(i, j - 1);
			candidates |= BOTTOM & (p != nullptr ? p->m_moves : zero);

			// bottom right
			p = getParticle(i + 1, j - 1);
			candidates |= BOTTOM_RIGHT & (p != nullptr ? p->m_moves : zero);

			// collect the candidates and their indices
			for (int i = 0; i < 8; ++i) {
				if (static_cast<bool>(candidates & std::byte(1 << i))) {
					indices.push_back(i);
				}
			}

			if (indices.size() == 0) continue;

			int choice = m_dist(m_rg) % indices.size();
			choice = indices[choice];
			pSwap(getIndex(i, j), getIndex(i + NEIGHBORS[choice][0], 
				j + NEIGHBORS[choice][1]));
		}
	}
	swapBuffers();
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

	//if (emptyBottom) {
	//	pSwap(getIndex(_x, _y), getIndex(_x, _y - 1));
	//	return;
	//} 
	int choice = m_dist(m_rg) % 3;
	choice = m_dist(m_rg) % 3;
	switch (choice) {
	case 0:
		if (emptyBottom) {
			pSwap(getIndex(_x, _y), getIndex(_x, _y - 1));
			return;
		} break;
	case 1:
		if (emptyBottomLeft) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y - 1));
			return;
		} break;
	case 2:
		if (emptyBottomRight) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y - 1));
			return;
		} break;
	}

	choice = m_dist(m_rg) % 3;
	switch (choice) {
	case 0:
		if (waterBottom) {
			pSwap(getIndex(_x, _y), getIndex(_x, _y - 1));
			return;
		} break;
	case 1:
		if (waterBottomLeft) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y - 1));
			return;
		} else if (waterLeft) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y));
			return;
		} break;
	case 2:
		if (waterBottomRight) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y - 1));
			return;
		} else if (waterRight) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y));
			return;
		}
		break;
	}
	noSwapUpdate(getIndex(_x, _y));
}

void Game::updateWater(int _x, int _y) {
	if ((*m_rPtr)[getIndex(_x, _y)].m_temp >= WATER_MAXTEMP) {
		steam(_x, _y);
		return;
	}

	bool emptyBottom = isEmpty(_x, _y - 1);
	bool emptyBottomLeft = isEmpty(_x - 1, _y - 1);
	bool emptyBottomRight = isEmpty(_x + 1, _y - 1);
	bool emptyLeft = isEmpty(_x - 1, _y);
	bool emptyRight = isEmpty(_x + 1, _y);

	if (emptyBottom) {
		pSwap(getIndex(_x, _y), getIndex(_x, _y - 1));
		return;
	}

	int choice = rand() % 2;
	switch (choice) {
	case 0:
		if (emptyBottomLeft) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y - 1)); 
			return;
		} else if (emptyLeft) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y)); 
			return;
		} break;
	case 1:
		if (emptyBottomRight) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y - 1)); 
			return;
		} else if (emptyRight) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y)); 
			return;
		} break;
	}
	noSwapUpdate(getIndex(_x, _y));
}

void Game::updateFire(int _x, int _y) {
	if ((*m_rPtr)[getIndex(_x, _y)].m_temp <= FIRE_MINTEMP) {
		//smoke(_x, _y);
		//return;
		setEmpty(_x, _y);
		return;
	} 

	uint8_t temp = (*m_rPtr)[getIndex(_x, _y)].m_temp;
	//m_particles[getIndex(_x, _y)].m_color.x = mymath::lerpU8(temp, 0, 1000, FIRE_COLOR[0], 255);
	//m_particles[getIndex(_x, _y)].m_color.y = 255 - mymath::lerpU8(temp, 0, 1000, 0, 255);
	uint8_t r = mymath::cxlerpu8(temp, FIRE_MINTEMP,
		FIRE_MAXTEMP, FIRE_R[0], FIRE_R[1]);
	uint8_t g = 150 - mymath::cxlerpu8(temp, FIRE_MINTEMP,
		FIRE_MAXTEMP, FIRE_G[0], FIRE_G[1]);
	
	//(*m_rPtr)[getIndex(_x, _y)].m_color.x = r;
	//(*m_rPtr)[getIndex(_x, _y)].m_color.y = g;

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
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y + 1));
		else if (emptyLeft)
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y));
		break;
	case 1:
		if (emptyTopRight)
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y + 1));
		else if (emptyRight)
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y));
		break;
	default:
		if (emptyTop) {
			pSwap(getIndex(_x, _y), getIndex(_x, _y + 1));
			return;
		}

		choice = m_dist(m_rg) % 2;
		switch (choice) {
		case 0:
			if(emptyBottomLeft)
				pSwap(getIndex(_x, _y), getIndex(_x - 1, _y - 1)); break;
		case 1:
			if(emptyBottomRight)
				pSwap(getIndex(_x, _y), getIndex(_x + 1, _y - 1)); break;
		}
		break;
	}
}

void Game::updateSmoke(int _x, int _y) {
	if ((*m_rPtr)[getIndex(_x, _y)].m_temp < SMOKE_MINTEMP) {
		setEmpty(_x, _y);
		return;
	} else if ((*m_rPtr)[getIndex(_x, _y)].m_temp > FIRE_MINTEMP) {
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
	int temp = (*m_rPtr)[getIndex(_x, _y)].m_temp;

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
	if ((*m_rPtr)[getIndex(_x, _y)].m_temp < WATER_BASETEMP) {
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

			switch (m_particleChoice) {
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
	(*m_wPtr)[getIndex(_x, _y)].m_id = WALL;
	(*m_wPtr)[getIndex(_x, _y)].m_tdiff = WALL_TDIFF;
	(*m_wPtr)[getIndex(_x, _y)].m_temp = WALL_BASETEMP;
	(*m_wPtr)[getIndex(_x, _y)].m_color.x = c;
	(*m_wPtr)[getIndex(_x, _y)].m_color.y = c;
	(*m_wPtr)[getIndex(_x, _y)].m_color.z = c;
}

void Game::sand(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dSand(m_rg));
	(*m_wPtr)[getIndex(_x, _y)].m_id = SAND;
	(*m_wPtr)[getIndex(_x, _y)].m_moves = MOVE_DOWNLEFT | MOVE_DOWN | MOVE_DOWNRIGHT;
	(*m_wPtr)[getIndex(_x, _y)].m_tdiff = SAND_TDIFF;
	(*m_wPtr)[getIndex(_x, _y)].m_temp = SAND_BASETEMP;
	(*m_wPtr)[getIndex(_x, _y)].m_color.x = SAND_COLOR[0];
	(*m_wPtr)[getIndex(_x, _y)].m_color.y = SAND_COLOR[1];
	(*m_wPtr)[getIndex(_x, _y)].m_color.z = SAND_COLOR[2];
}

void Game::water(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dWater(m_rg));
	(*m_wPtr)[getIndex(_x, _y)].m_id = WATER;
	(*m_wPtr)[getIndex(_x, _y)].m_tdiff = WATER_TDIFF;
	(*m_wPtr)[getIndex(_x, _y)].m_temp = WATER_BASETEMP;
	(*m_wPtr)[getIndex(_x, _y)].m_color.x = WATER_COLOR[0];
	(*m_wPtr)[getIndex(_x, _y)].m_color.y = WATER_COLOR[1];
	(*m_wPtr)[getIndex(_x, _y)].m_color.z = WATER_COLOR[2];
}

void Game::fire(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dFire(m_rg));
	(*m_wPtr)[getIndex(_x, _y)].m_id = FIRE;
	(*m_wPtr)[getIndex(_x, _y)].m_tdiff = FIRE_TDIFF;
	(*m_wPtr)[getIndex(_x, _y)].m_temp = FIRE_BASETEMP;
	(*m_wPtr)[getIndex(_x, _y)].m_color.x = FIRE_COLOR[0];
	(*m_wPtr)[getIndex(_x, _y)].m_color.y = FIRE_COLOR[1];
	(*m_wPtr)[getIndex(_x, _y)].m_color.z = FIRE_COLOR[2];
}

void Game::smoke(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dWall(m_rg));
	(*m_wPtr)[getIndex(_x, _y)].m_id = SMOKE;
	(*m_wPtr)[getIndex(_x, _y)].m_tdiff = SMOKE_TDIFF;
	(*m_wPtr)[getIndex(_x, _y)].m_temp = SMOKE_BASETEMP;
	(*m_wPtr)[getIndex(_x, _y)].m_color.x = SMOKE_COLOR[0];
	(*m_wPtr)[getIndex(_x, _y)].m_color.y = SMOKE_COLOR[1];
	(*m_wPtr)[getIndex(_x, _y)].m_color.z = SMOKE_COLOR[2];
}

void Game::metal(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dWall(m_rg));
	(*m_wPtr)[getIndex(_x, _y)].m_id = METAL;
	(*m_wPtr)[getIndex(_x, _y)].m_tdiff = METAL_TDIFF;
	(*m_wPtr)[getIndex(_x, _y)].m_temp = METAL_BASETEMP;
	(*m_wPtr)[getIndex(_x, _y)].m_color.x = METAL_COLOR[0];
	(*m_wPtr)[getIndex(_x, _y)].m_color.y = METAL_COLOR[1];
	(*m_wPtr)[getIndex(_x, _y)].m_color.z = METAL_COLOR[2];
}

void Game::steam(int _x, int _y) {
	(*m_wPtr)[getIndex(_x, _y)].m_id = STEAM;
	(*m_wPtr)[getIndex(_x, _y)].m_tdiff = STEAM_TDIFF;
	(*m_wPtr)[getIndex(_x, _y)].m_temp = STEAM_BASETEMP;
	(*m_wPtr)[getIndex(_x, _y)].m_color.x = 100;
	(*m_wPtr)[getIndex(_x, _y)].m_color.y = 100;
	(*m_wPtr)[getIndex(_x, _y)].m_color.z = 180;
}

void Game::torch(int _x, int _y) {
	(*m_wPtr)[getIndex(_x, _y)].m_id = TORCH;
	(*m_wPtr)[getIndex(_x, _y)].m_tdiff = TORCH_TDIFF;
	(*m_wPtr)[getIndex(_x, _y)].m_temp = TORCH_BASETEMP;
	(*m_wPtr)[getIndex(_x, _y)].m_color.x = 150;
	(*m_wPtr)[getIndex(_x, _y)].m_color.y = 150;
	(*m_wPtr)[getIndex(_x, _y)].m_color.z = 0;
}
