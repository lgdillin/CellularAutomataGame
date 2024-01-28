#include "Game.hpp"

Game::Game() {
	std::random_device rd;
	m_randomGen = std::mt19937(rd());
	m_dWall = std::uniform_int_distribution<int>(40, 50);
	m_dSand = std::uniform_int_distribution<int>(120, 150);
	m_dWater = std::uniform_int_distribution<int>(120, 150);
	m_dFire = std::uniform_int_distribution<int>(20, 40);

	m_particles = std::vector<Particle>(TEXTURE_ROWS * TEXTURE_COLS);

	for (size_t i = 0; i < TEXTURE_ROWS; ++i) {
		bool coinFlip = (rand() > RAND_MAX / 2);
		if (coinFlip) continue;
		coinFlip = (rand() > RAND_MAX / 2);
		int adjust = 0;
		if (coinFlip) adjust = 10;

		m_particles[(TEXTURE_ROWS * TEXTURE_COLS - TEXTURE_ROWS) + i].m_id = SAND;
		m_particles[(TEXTURE_ROWS * TEXTURE_COLS - TEXTURE_ROWS) + i].m_color.x 
			= static_cast<uint8_t>(m_dSand(m_randomGen) + 50);
		m_particles[(TEXTURE_ROWS * TEXTURE_COLS - TEXTURE_ROWS) + i].m_color.y 
			= static_cast<uint8_t>(m_dSand(m_randomGen));
	}
}

Game::~Game() {
}

void Game::initialize() {

}

void Game::update() {
	for (size_t i = 0; i < TEXTURE_COLS; ++i) {
		for (size_t j = 0; j < TEXTURE_ROWS; ++j) {
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
			}
		}
	}
}

void Game::addParticle() {
	int scalex = static_cast<int>((m_mousex / (float)SCREEN_WIDTH) * TEXTURE_COLS);
	// SDL mouse input has y increase at the bottom of window, so flip
	int scaley = static_cast<int>((m_mousey / (float)SCREEN_HEIGHT) * TEXTURE_ROWS);
	scaley = (TEXTURE_ROWS)- scaley;

	scalex = scalex >= TEXTURE_COLS ? TEXTURE_COLS - 1 : scalex;
	scalex = scalex < 0 ? 0 : scalex;
	scaley = scaley >= TEXTURE_ROWS ? TEXTURE_ROWS - 1 : scaley;
	scaley = scaley < 0 ? 0 : scaley;

	//std::cout << scalex << " " << scaley << std::endl;

	uint8_t c;
	switch (m_particleChoice) {
	case WALL:
		addWall(scalex, scaley);
		break;
	case SAND:
		addSand(scalex, scaley);
		break;
	case WATER:
		addWater(scalex, scaley);
		break;
	case FIRE:
		addFire(scalex, scaley);
		break;
	case METAL:
		addMetal(scalex, scaley);
	default:
		break;
	}
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
		r = rand() % 3;
		if(r > 0) pSwap(getIndex(_x, _y), getIndex(_x, _y - 1));
		return;
	}

	if (coinFlip) {
		r = rand() % 3;
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
	int choice = rand() % 3;
	bool emptyBottom = isEmpty(_x, _y - 1);
	bool emptyBottomLeft = isEmpty(_x - 1, _y - 1);
	bool emptyBottomRight = isEmpty(_x + 1, _y - 1);

	//if (emptyBottom || emptyBottomLeft || emptyBottomRight) {
	//	pSwap(getIndex(_x, _y), getIndex(_x, _y - 1));
	//	return;
	//}


	if (choice == 0) {
		if (isEmpty(_x - 1, _y)) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y));
		}
	} else if (choice == 1) {
		if (isEmpty(_x + 1, _y)) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y));
		}
	} else {
		if (isEmpty(_x, _y - 1)) {
			pSwap(getIndex(_x, _y), getIndex(_x, _y - 1));
		}
	}
}

void Game::updateFire(int _x, int _y) {
	if (m_particles[getIndex(_x, _y)].m_life <= 0) {
		uint8_t c = static_cast<uint8_t>(m_dWall(m_randomGen));
		m_particles[getIndex(_x, _y)].m_id = SMOKE;
		m_particles[getIndex(_x, _y)].m_color.x = c - 5;
		m_particles[getIndex(_x, _y)].m_color.y = c - 5;
		m_particles[getIndex(_x, _y)].m_color.z = c - 5;
		m_particles[getIndex(_x, _y)].m_life = 20;
		return;
	} else if (m_particles[getIndex(_x, _y)].m_life > 0){
		--m_particles[getIndex(_x, _y)].m_life;
		m_particles[getIndex(_x, _y)].m_temp = 220;
	}

	int choice = rand() % 10;
	switch (choice) {
	case 0:
		if (isEmpty(_x - 1, _y + 1) || isSmoke(_x, _y)) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y + 1));
		} else if (!isEmpty(_x - 1, _y + 1) && isEmpty(_x - 1, _y)) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y));
		}
		break;
	case 1:
		if (isEmpty(_x + 1, _y + 1) || isSmoke(_x + 1, _y + 1)) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y + 1));
		} else if (!isEmpty(_x + 1, _y + 1) && isEmpty(_x + 1, _y)) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y));
		}
		break;
	default:
		if (isEmpty(_x, _y + 1) || isSmoke(_x, _y + 1)) {
			pSwap(getIndex(_x, _y), getIndex(_x, _y + 1));
		}
		break;
	}

	if (!isEmpty(_x - 1, _y - 1)) {
		avgTemps(getIndex(_x - 1, _y - 1), getIndex(_x, _y));
	}

	if (!isEmpty(_x, _y - 1)) {
		avgTemps(getIndex(_x, _y - 1), getIndex(_x, _y));
	}

	if (!isEmpty(_x + 1, _y - 1)) {
		avgTemps(getIndex(_x + 1, _y - 1), getIndex(_x, _y));
	}

	if (!isEmpty(_x - 1, _y)) {
		avgTemps(getIndex(_x - 1, _y), getIndex(_x, _y));
	}

	if (!isEmpty(_x + 1, _y)) {
		avgTemps(getIndex(_x + 1, _y), getIndex(_x, _y));
	}

	if (!isEmpty(_x - 1, _y + 1)) {
		avgTemps(getIndex(_x - 1, _y + 1), getIndex(_x, _y));
	}

	if (!isEmpty(_x, _y + 1)) {
		avgTemps(getIndex(_x, _y + 1), getIndex(_x, _y));
	}

	if (!isEmpty(_x + 1, _y + 1)) {
		avgTemps(getIndex(_x + 1, _y + 1), getIndex(_x, _y));
	}

}

void Game::updateSmoke(int _x, int _y) {
	if (m_particles[getIndex(_x, _y)].m_life <= 0) {
		setEmpty(_x, _y);
		return;
	} else {
		--m_particles[getIndex(_x, _y)].m_life;
		uint8_t c = m_particles[getIndex(_x, _y)].m_color.x;
		c = c > 10 ? c - 10 : 10;
		m_particles[getIndex(_x, _y)].m_color.x = c;
		m_particles[getIndex(_x, _y)].m_color.y = c;
		m_particles[getIndex(_x, _y)].m_color.z = c;
	}

	int choice = rand() % 3;
	switch (choice) {
	case 0:
		if (isEmpty(_x - 1, _y + 1)) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y + 1));
		} else if (isEmpty(_x - 1, _y)) {
			pSwap(getIndex(_x, _y), getIndex(_x - 1, _y));
		}
		break;
	case 1:
		if (isEmpty(_x + 1, _y + 1)) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y + 1));
		} else if (isEmpty(_x + 1, _y)) {
			pSwap(getIndex(_x, _y), getIndex(_x + 1, _y));
		}
		break;
	default:
		if (isEmpty(_x, _y + 1)) {
			pSwap(getIndex(_x, _y), getIndex(_x, _y + 1));
		}
		break;
	}
}

void Game::updateMetal(int _x, int _y) {
	bool metalBottom = getId(_x, _y - 1) == METAL;
	bool metalBottomLeft = getId(_x - 1, _y - 1) == METAL;
	bool metalBottomRight = getId(_x + 1, _y - 1) == METAL;
	bool metalLeft = getId(_x - 1, _y) == METAL;
	bool metalRight = getId(_x + 1, _y) == METAL;
	bool metalTopLeft = getId(_x - 1, _y + 1) == METAL;
	bool metalTop = getId(_x, _y + 1) == METAL;
	bool metalTopRight = getId(_x + 1, _y + 1) == METAL;

	uint8_t temp = m_particles[getIndex(_x, _y)].m_temp;
	uint8_t c = m_particles[getIndex(_x, _y)].m_color.x;
	//m_particles[getIndex(_x, _y)].m_color.x = mymath::min(mymath::abs(c + 0.1 * temp), 255);
	//m_particles[getIndex(_x, _y)].m_color.y = mymath::min(mymath::abs(c - 0.1 * temp), 255);
	//m_particles[getIndex(_x, _y)].m_color.z = mymath::min(mymath::abs(c - 0.1 * temp), 255);

	uint8_t nTemp;
	if (metalBottom) {
		nTemp = static_cast<uint8_t>(
			(temp + m_particles[getIndex(_x, _y - 1)].m_temp) / 2);
		m_particles[getIndex(_x, _y)].m_temp = nTemp;
		m_particles[getIndex(_x, _y - 1)].m_temp = nTemp;
	}

	if (metalBottomLeft) {
		nTemp = static_cast<uint8_t>(
			(temp + m_particles[getIndex(_x - 1, _y - 1)].m_temp) / 2);
		m_particles[getIndex(_x, _y)].m_temp = nTemp;
		m_particles[getIndex(_x - 1, _y - 1)].m_temp = nTemp;
	}

	if (metalBottomRight) {
		nTemp = static_cast<uint8_t>(
			(temp + m_particles[getIndex(_x + 1, _y - 1)].m_temp) / 2);
		m_particles[getIndex(_x, _y)].m_temp = nTemp;
		m_particles[getIndex(_x + 1, _y - 1)].m_temp = nTemp;
	}

	if (metalLeft) {
		nTemp = static_cast<uint8_t>(
			(temp + m_particles[getIndex(_x - 1, _y)].m_temp) / 2);
		m_particles[getIndex(_x, _y)].m_temp = nTemp;
		m_particles[getIndex(_x - 1, _y)].m_temp = nTemp;
	}

	if (metalRight) {
		nTemp = static_cast<uint8_t>(
			(temp + m_particles[getIndex(_x + 1, _y)].m_temp) / 2);
		m_particles[getIndex(_x, _y)].m_temp = nTemp;
		m_particles[getIndex(_x + 1, _y)].m_temp = nTemp;
	}

	if (metalTopLeft) {
		nTemp = static_cast<uint8_t>(
			(temp + m_particles[getIndex(_x - 1, _y + 1)].m_temp) / 2);
		m_particles[getIndex(_x, _y)].m_temp = nTemp;
		m_particles[getIndex(_x - 1, _y + 1)].m_temp = nTemp;
	}

	if (metalTop) {
		nTemp = static_cast<uint8_t>(
			(temp + m_particles[getIndex(_x, _y + 1)].m_temp) / 2);
		m_particles[getIndex(_x, _y)].m_temp = nTemp;
		m_particles[getIndex(_x, _y + 1)].m_temp = nTemp;
	}

	if (metalTopRight) {
		nTemp = static_cast<uint8_t>(
			(temp + m_particles[getIndex(_x + 1, _y + 1)].m_temp) / 2);
		m_particles[getIndex(_x, _y)].m_temp = nTemp;
		m_particles[getIndex(_x + 1, _y + 1)].m_temp = nTemp;
	}

	// do some equilibrium
	uint8_t t = m_particles[getIndex(_x, _y)].m_temp;
	t = t > 70 ? t - 1 : 70;
	m_particles[getIndex(_x, _y)].m_temp = t;
	m_particles[getIndex(_x, _y)].m_color.x = t;

}

void Game::pSwap(size_t _idx1, size_t _idx2) {
	Particle tmp = m_particles[_idx1];

	m_particles[_idx1].m_id = m_particles[_idx2].m_id;
	m_particles[_idx1].m_color = m_particles[_idx2].m_color;
	m_particles[_idx1].m_life = m_particles[_idx2].m_life;
	m_particles[_idx1].m_temp = m_particles[_idx2].m_temp;

	m_particles[_idx2].m_id = tmp.m_id;
	m_particles[_idx2].m_color = tmp.m_color;
	m_particles[_idx2].m_life = tmp.m_life;
	m_particles[_idx2].m_temp = tmp.m_temp;
}

void Game::setEmpty(int _x, int _y) {
	m_particles[getIndex(_x, _y)].m_id = EMPTY;
	m_particles[getIndex(_x, _y)].m_life = 255;
	m_particles[getIndex(_x, _y)].m_color.x = 0;
	m_particles[getIndex(_x, _y)].m_color.y = 0;
	m_particles[getIndex(_x, _y)].m_color.z = 0;
}

void Game::addWall(int _x, int _y) {
	uint8_t c;
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			c = static_cast<uint8_t>(m_dWall(m_randomGen));
			m_particles[getIndex(_x + i, _y + j)].m_id = WALL;
			m_particles[getIndex(_x + i, _y + j)].m_color.x = c;
			m_particles[getIndex(_x + i, _y + j)].m_color.y = c;
			m_particles[getIndex(_x + i, _y + j)].m_color.z = c;
		}
	}
}

void Game::addSand(int _x, int _y) {
	uint8_t c;
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			c = static_cast<uint8_t>(m_dSand(m_randomGen));
			m_particles[getIndex(_x + i, _y + j)].m_id = SAND;
			m_particles[getIndex(_x + i, _y + j)].m_color.x = c + 50;
			m_particles[getIndex(_x + i, _y + j)].m_color.y = c;
		}
	}
}

void Game::addWater(int _x, int _y) {
	uint8_t c;
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			c = static_cast<uint8_t>(m_dWater(m_randomGen));
			m_particles[getIndex(_x + i, _y+ j)].m_id = WATER;
			m_particles[getIndex(_x + i, _y+ j)].m_color.z = c;
		}
	}
}

void Game::addFire(int _x, int _y) {
	uint8_t c;
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			c = static_cast<uint8_t>(m_dFire(m_randomGen));
			m_particles[getIndex(_x + i, _y + j)].m_id = FIRE;
			m_particles[getIndex(_x + i, _y + j)].m_life = 15;
			m_particles[getIndex(_x + i, _y + j)].m_color.x = c + 120;
			m_particles[getIndex(_x + i, _y + j)].m_color.y = c;
			m_particles[getIndex(_x + i, _y + j)].m_temp = 220;
		}
	}
}

void Game::addMetal(int _x, int _y) {
	uint8_t c;
	for (int i = -1; i <= 1; ++i) {
		for (int j = -1; j <= 1; ++j) {
			c = static_cast<uint8_t>(m_dWall(m_randomGen));
			m_particles[getIndex(_x + i, _y + j)].m_id = METAL;
			m_particles[getIndex(_x + i, _y + j)].m_color.x = 70;
			m_particles[getIndex(_x + i, _y + j)].m_color.y = 70;
			m_particles[getIndex(_x + i, _y + j)].m_color.z = 70;
			m_particles[getIndex(_x + i, _y + j)].m_temp = 70;
		}
	}
}
