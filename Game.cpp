#include "Game.hpp"

Game::Game() {
	m_updateLatch = 0;
	std::random_device rd;
	m_rg = std::mt19937(rd());
	m_dWall = std::uniform_int_distribution<int>(40, 50);
	m_dSand = std::uniform_int_distribution<int>(120, 150);
	m_dWater = std::uniform_int_distribution<int>(120, 150);
	m_dFire = std::uniform_int_distribution<int>(20, 40);
	m_dist = std::uniform_int_distribution<int>(0, 99);

	m_particles = std::vector<Particle>(TEXTURE_ROWS * TEXTURE_COLS);
	m_particleChoice = 0;

	m_mousex = 0;
	m_mousey = 0;
	m_brushSize = 1;

	//for (size_t i = 0; i < TEXTURE_ROWS; ++i) {
	//	bool coinFlip = (rand() > RAND_MAX / 2);
	//	if (coinFlip) continue;
	//	coinFlip = (rand() > RAND_MAX / 2);
	//	int adjust = 0;
	//	if (coinFlip) adjust = 10;

	//	m_particles[(TEXTURE_ROWS * TEXTURE_COLS - TEXTURE_ROWS) + i].m_id = SAND;
	//	m_particles[(TEXTURE_ROWS * TEXTURE_COLS - TEXTURE_ROWS) + i].m_color.x 
	//		= static_cast<uint8_t>(m_dSand(m_randomGen) + 50);
	//	m_particles[(TEXTURE_ROWS * TEXTURE_COLS - TEXTURE_ROWS) + i].m_color.y 
	//		= static_cast<uint8_t>(m_dSand(m_randomGen));
	//	
	//}
}

Game::~Game() {
}

void Game::initialize() {

}

void Game::update() {
	for (int i = 0; i < TEXTURE_COLS; ++i) {
		for (int j = 0; j < TEXTURE_ROWS; ++j) {
			if (m_particles[j * TEXTURE_ROWS + i].m_latch == m_updateLatch) continue;
			//flipLatch(i, j);

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
			updateTemp(i, j);
		}
	}

	flipUpdateLatch();
}

void Game::updateTemp(int _x, int _y) {

	int cp_temp = 0;
	int cp = 0;
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			if (!inBounds(_x + i, _y + j)) continue;
			Particle p = m_particles[getIndex(_x + i, _y + j)];

			cp_temp += (p.m_specHeat * p.m_temp);
			cp += p.m_specHeat;
		}
	}

	int tempFinal = (float)cp_temp / cp < MAX_U16BIT ? (float)cp_temp / cp : MAX_U16BIT;
	m_particles[getIndex(_x, _y)].m_temp = static_cast<uint16_t>(tempFinal);
}

void Game::updateSand(int _x, int _y) {
	int r = rand();
	bool coinFlip = r > RAND_MAX / 2;
	bool emptyBottom = isEmpty(_x, _y - 1);
	bool emptyBottomLeft = isEmpty(_x - 1, _y - 1);
	bool emptyBottomRight = isEmpty(_x + 1, _y - 1);
	bool waterBottom = isWater(_x, _y - 1);
	bool waterBottomLeft = isWater(_x - 1, _y - 1);
	bool waterBottomRight = isWater(_x + 1, _y - 1);

	if (emptyBottom) {
		pSwap(getIndex(_x, _y), getIndex(_x, _y - 1));
		return;
	} 

	if (waterBottom) {
		r = rand() % 15;
		if(r > 0) pSwap(getIndex(_x, _y), getIndex(_x, _y - 1));
		return;
	}

	if (coinFlip) {
		r = rand() % 15;
		if (waterBottomLeft) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y - 1));
			return;
		} else if (waterBottomRight) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y - 1));
			return;
		}
	}

	if (coinFlip) {
		if (emptyBottomLeft) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y - 1));
		}
	} else {
		if (emptyBottomRight) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y - 1));
		}
	}
}

void Game::updateWater(int _x, int _y) {
	if (m_particles[getIndex(_x, _y)].m_temp >= WATER_MAXTEMP) {
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
		if(emptyBottomLeft)
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y - 1));
		else if(emptyLeft)
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y));
		break;
	case 1:
		if(emptyBottomRight)
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y - 1));
		else if(emptyRight)
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y));
		break;
	}
}

void Game::updateFire(int _x, int _y) {
	if (m_particles[getIndex(_x, _y)].m_temp <= FIRE_MINTEMP) {
		smoke(_x, _y);
		return;
	} 
	int temp = m_particles[getIndex(_x, _y)].m_temp;
	//m_particles[getIndex(_x, _y)].m_color.x = temp < 255 ? temp : 255;
	//m_particles[getIndex(_x, _y)].m_color.z = temp

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
		if (emptyTop)
			pSwap(getIndex(_x, _y), getIndex(_x, _y + 1));
		else if (emptyBottomLeft)
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y - 1));
		else if (emptyBottomRight)
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y - 1));

		if (isSmoke(_x, _y + 1)) {
			//swapEmpty(getIndex(_x, _y), getIndex(_x, _y + 1));
		}

		break;
	}
}

void Game::updateSmoke(int _x, int _y) {
	if (m_particles[getIndex(_x, _y)].m_temp < SMOKE_MINTEMP) {
		setEmpty(_x, _y);
		return;
	}  else if (m_particles[getIndex(_x, _y)].m_temp > FIRE_MINTEMP) {
		fire(_x, _y);
	} else {
		uint8_t c = m_particles[getIndex(_x, _y)].m_color.x;
		c = c > 10 ? c - 10 : 10;
		m_particles[getIndex(_x, _y)].m_color.x = c;
		m_particles[getIndex(_x, _y)].m_color.y = c;
		m_particles[getIndex(_x, _y)].m_color.z = c;
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
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y ));
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
	int temp = m_particles[getIndex(_x, _y)].m_temp - METAL_BASETEMP;


	if (temp < 100) {
		m_particles[getIndex(_x, _y)].m_color.x = 70 + temp;
	} else {
		m_particles[getIndex(_x, _y)].m_color.x = 70 + temp;
		m_particles[getIndex(_x, _y)].m_color.y = 70 + temp;
		m_particles[getIndex(_x, _y)].m_color.z = 70 + temp;
	}

}

void Game::updateSteam(int _x, int _y) {
	if (m_particles[getIndex(_x, _y)].m_temp < WATER_BASETEMP) {
		water(_x, _y);
		return;
	}

	int choice = rand() % 5;
	switch (choice) {
	case 0:
		if (isEmpty(_x, _y + 1) || isSmoke(_x, _y + 1) || isWater(_x, _y + 1)) {
			pSwap(getIndex(_x, _y), getIndex(_x, _y + 1));
		}
		break;
	case 1:
	case 2:
		if (isEmpty(_x - 1, _y + 1) || isSmoke(_x, _y) || isWater(_x, _y)) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y + 1));
		} else if (!isEmpty(_x - 1, _y + 1) && isEmpty(_x - 1, _y)) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y));
		}
		break;
	case 3:
	case 4:
		if (isEmpty(_x + 1, _y + 1) || isSmoke(_x + 1, _y + 1) || isWater(_x + 1, _y + 1)) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y + 1));
		} else if (!isEmpty(_x + 1, _y + 1) && isEmpty(_x + 1, _y)) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y));
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

void Game::setEmpty(int _x, int _y) {
	m_particles[getIndex(_x, _y)].m_id = EMPTY;
	m_particles[getIndex(_x, _y)].m_specHeat = EMPTY_SPECIFICHEAT;
	m_particles[getIndex(_x, _y)].m_temp = EMPTY_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = 0;
	m_particles[getIndex(_x, _y)].m_color.y = 0;
	m_particles[getIndex(_x, _y)].m_color.z = 0;
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

	uint8_t c;
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
	m_particles[getIndex(_x, _y)].m_id = WALL;
	m_particles[getIndex(_x, _y)].m_specHeat = WALL_SPECIFICHEAT;
	m_particles[getIndex(_x, _y)].m_temp = WALL_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = c;
	m_particles[getIndex(_x, _y)].m_color.y = c;
	m_particles[getIndex(_x, _y)].m_color.z = c;
}

void Game::sand(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dSand(m_rg));
	m_particles[getIndex(_x, _y)].m_id = SAND;
	m_particles[getIndex(_x, _y)].m_specHeat = SAND_SPECIFICHEAT;
	m_particles[getIndex(_x, _y)].m_temp = SAND_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = c + 50;
	m_particles[getIndex(_x, _y)].m_color.y = c;
	m_particles[getIndex(_x, _y)].m_color.z = 20;
}

void Game::water(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dWater(m_rg));
	m_particles[getIndex(_x, _y)].m_id = WATER;
	m_particles[getIndex(_x, _y)].m_specHeat = WATER_SPECIFICHEAT;
	m_particles[getIndex(_x, _y)].m_temp = WATER_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = 20;
	m_particles[getIndex(_x, _y)].m_color.y = 20;
	m_particles[getIndex(_x, _y)].m_color.z = c;
}

void Game::fire(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dFire(m_rg));
	m_particles[getIndex(_x, _y)].m_id = FIRE;
	m_particles[getIndex(_x, _y)].m_specHeat = FIRE_SPECIFICHEAT;
	m_particles[getIndex(_x, _y)].m_temp = FIRE_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = 150;
	m_particles[getIndex(_x, _y)].m_color.y = 150;
	m_particles[getIndex(_x, _y)].m_color.y = 10;
}

void Game::smoke(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dWall(m_rg));
	m_particles[getIndex(_x, _y)].m_id = SMOKE;
	m_particles[getIndex(_x, _y)].m_specHeat = SMOKE_SPECIFICHEAT;
	m_particles[getIndex(_x, _y)].m_temp = SMOKE_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = c - 5;
	m_particles[getIndex(_x, _y)].m_color.y = c - 5;
	m_particles[getIndex(_x, _y)].m_color.z = c - 5;
}

void Game::metal(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dWall(m_rg));
	m_particles[getIndex(_x, _y)].m_id = METAL;
	m_particles[getIndex(_x, _y)].m_specHeat = METAL_SPECIFICHEAT;
	m_particles[getIndex(_x, _y)].m_temp = METAL_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = 70;
	m_particles[getIndex(_x, _y)].m_color.y = 70;
	m_particles[getIndex(_x, _y)].m_color.z = 70;
}

void Game::steam(int _x, int _y) {
	m_particles[getIndex(_x, _y)].m_id = STEAM;
	m_particles[getIndex(_x, _y)].m_specHeat = STEAM_SPEFICHEAT;
	m_particles[getIndex(_x, _y)].m_temp = STEAM_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = 100;
	m_particles[getIndex(_x, _y)].m_color.y = 100;
	m_particles[getIndex(_x, _y)].m_color.z = 180;
}

void Game::torch(int _x, int _y) {
	m_particles[getIndex(_x, _y)].m_id = TORCH;
	m_particles[getIndex(_x, _y)].m_specHeat = TORCH_SPECIFICHEAT;
	m_particles[getIndex(_x, _y)].m_temp = TORCH_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = 150;
	m_particles[getIndex(_x, _y)].m_color.y = 150;
	m_particles[getIndex(_x, _y)].m_color.z = 0;
}
