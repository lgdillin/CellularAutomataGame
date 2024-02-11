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
	for (int row = 0; row < TEXTURE_ROWS; ++row) {
		for (int col = 0; col < TEXTURE_COLS; ++col) {
			if (row == 0 || col == 0 || row == TEXTURE_ROWS - 1 || col == TEXTURE_COLS - 1) {
				m_particles[row * TEXTURE_ROWS + col].m_id = OOB;
				m_particles[row * TEXTURE_ROWS + col].m_temp = 295;
				m_particles[row * TEXTURE_ROWS + col].m_color.x = 255;
				m_particles[row * TEXTURE_ROWS + col].m_color.y = 255;
				m_particles[row * TEXTURE_ROWS + col].m_color.z = 255;

				// initialize border tempgrid
				m_tempGrid[row * TEXTURE_ROWS + col] = 295;
			}
		}
	}
}

Game::~Game() {
}

void Game::initialize() {

}

void Game::update2() {
	for (int i = 0; i < TEXTURE_ROWS; ++i) {
		for (int j = 0; j < TEXTURE_COLS; ++j) {
			if (i == 0 || j == 0 || j == TEXTURE_COLS - 1 || i == TEXTURE_ROWS - 1)
				continue;

			switch (m_particles[j * TEXTURE_ROWS + i].m_id) {
			case OOB:
				std::cout << "OOB: " << i << ", " << j << std::endl;
				break;
			case EMPTY:
				updateEmpty(i, j);
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
			case ACID:
				updateAcid(i, j);
				break;

			case TORCH:
				updateTorch(i, j);
				break;
			}

		}
	}

	commitUpdate();
}

void Game::commitUpdate() {
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

	updateTemps();
}

void Game::updateTemps() {
	float c, t, l, r, b, tn1;
	float tl, tr, bl, br;
	for (int row = 0; row < TEXTURE_ROWS; ++row) {
		for (int col = 0; col < TEXTURE_COLS; ++col) {
			if (row == 0 || col == 0 || row == TEXTURE_ROWS - 1 || col == TEXTURE_COLS - 1)
				continue;

			tl = getTemp2(col - 1, row + 1);
			tr = getTemp2(col + 1, row + 1);
			bl = getTemp2(col - 1, row - 1);
			br = getTemp2(col + 1, row - 1);

			c = getTemp2(col, row);
			t = getTemp2(col, row + 1);
			l = getTemp2(col - 1, row);
			r = getTemp2(col + 1, row);
			b = getTemp2(col, row - 1);

			tn1 = (c + (tl + t + tr + l + r + bl + b + br))
				/ (9.0f);
			float delta_t = tn1 - c;
			float newTemp = c + THERM_COEF(getId(col, row)) * delta_t;
			
			//tn1 = (c / (9.0f - coef)) + ((tl + t + tr + l + r + bl + b + br) / (9.0f + coef));
			m_tempGrid[row * TEXTURE_ROWS + col] = static_cast<uint16_t>(newTemp);
		}
	}

	for (int i = 0; i < m_particles.size(); ++i) {
		m_particles[i].m_temp = m_tempGrid[i];
	}
}

void Game::updateEmpty(int _x, int _y) {
	float t = m_particles[getIndex(_x, _y)].m_temp;
	if (t < 300) return;

	bool emptyTop = isEmpty(_x, _y + 1);
	bool emptyTopLeft = isEmpty(_x - 1, _y + 1);
	bool emptyTopRight = isEmpty(_x + 1, _y + 1);

	if (emptyTop && t > getTemp2(_x, _y + 1)) {
		storeChange(_x, _y, _x, _y + 1);
		return;
	}

	int choice = m_dist(m_rg) % 2;
	switch (choice) {
	case 0:
		if (emptyTopLeft && t > getTemp2(_x - 1, _y + 1)) {
			storeChange(_x, _y, _x - 1, _y + 1);
			return;
		} break;
	case 1:
		if (emptyTopRight && t > getTemp2(_x + 1, _y + 1)) {
			storeChange(_x, _y, _x + 1, _y + 1);
			return;
		} break;
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
	//case 3:
	//	if (waterLeft) {
	//		storeChange(_x, _y, _x - 1, _y);
	//		return;
	//	} break;
	//case 4:
	//	if (waterRight) {
	//		storeChange(_x, _y, _x + 1, _y);
	//		return;
	//	} break;
	}
}

void Game::updateWater(int _x, int _y) {
	int choice;
	float t = m_particles[getIndex(_x, _y)].m_temp;
	float smooth;

	if (t < 400) {
		m_particles[getIndex(_x, _y)].m_color.x = 20;
		m_particles[getIndex(_x, _y)].m_color.y = 20;
		m_particles[getIndex(_x, _y)].m_color.z = 130;
	} else if (t >= 400 && t < 500) {
		smooth = (t - 400) / 100;
		m_particles[getIndex(_x, _y)].m_color.x = mymath::lerp(20, 150, t);
		m_particles[getIndex(_x, _y)].m_color.y = mymath::lerp(20, 150, t);
		m_particles[getIndex(_x, _y)].m_color.z = mymath::lerp(130, 170, t);
	} else if (t >= 500) {
		m_particles[getIndex(_x, _y)].m_color.x = 150;
		m_particles[getIndex(_x, _y)].m_color.y = 150;
		m_particles[getIndex(_x, _y)].m_color.z = 170;
	}

	bool waterTop = isWater(_x, _y + 1);
	bool waterTopLeft = isWater(_x - 1, _y + 1);
	bool waterTopRight = isWater(_x + 1, _y + 1);
	//choice = m_dist(m_rg) % 3;
	//switch (choice) {
	//case 0:
	//	if (waterTop && t > getTemp2(_x, _y + 1)) {
	//		storeChange(_x, _y, _x, _y + 1);
	//		return;
	//	}
	//	break;
	//case 1:
	//	if (waterTopLeft && t > getTemp2(_x - 1, _y + 1)) {
	//		storeChange(_x, _y, _x - 1, _y + 1);
	//		return;
	//	}
	//	break;
	//case 2:
	//	if (waterTopRight && t > getTemp(_x + 1, _y + 1)) {
	//		storeChange(_x, _y, _x + 1, _y + 1);
	//		return;
	//	}
	//	break;
	//}



	bool emptyTop = isEmpty(_x, _y + 1);
	bool emptyTopLeft = isEmpty(_x - 1, _y + 1);
	bool emptyTopRight = isEmpty(_x + 1, _y + 1);
	bool emptyBottom = isEmpty(_x, _y - 1);
	bool emptyBottomLeft = isEmpty(_x - 1, _y - 1);
	bool emptyBottomRight = isEmpty(_x + 1, _y - 1);
	bool emptyLeft = isEmpty(_x - 1, _y);
	bool emptyRight = isEmpty(_x + 1, _y);

	// The get temp thing isn't working because getTemp2 doesn't check 
	// if the ids match. so we have to nest the temp check inside the id check,
	// or just make a whole function
	
	bool steamState = t >= 500 ? true : false;
	switch (steamState) {
	case true:
		choice = m_dist(m_rg) % 3;
		switch (choice) {
		case 0:
			if (emptyTopLeft || (waterTopLeft && t > getTemp2(_x - 1, _y + 1)))
				storeChange(_x, _y, _x - 1, _y + 1);
			else if (emptyLeft)
				storeChange(_x, _y, _x - 1, _y);
			break;
		case 1:
			if (emptyTopRight || (waterTopRight && t > getTemp2(_x + 1, _y + 1)))
				storeChange(_x, _y, _x + 1, _y + 1);
			else if (emptyRight)
				storeChange(_x, _y, _x + 1, _y);
			break;
		case 2:
			if (emptyTop || (waterTop && t > getTemp2(_x, _y + 1))) {
				storeChange(_x, _y, _x, _y + 1);
				return;
			}

			choice = m_dist(m_rg) % 2;
			switch (choice) {
			case 0:
				if (emptyBottomLeft)
					storeChange(_x, _y, _x - 1, _y - 1); break;
			case 1:
				if (emptyBottomRight)
					storeChange(_x, _y, _x + 1, _y - 1); break;
			}
			break;
		}
		return;
		break;
	case false:
		if (emptyBottom) {
			storeChange(_x, _y, _x, _y - 1);
			return;
		}

		choice = rand() % 2;
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
		
		choice = rand() % 3;
		switch (choice) {
		case 0:
			if (waterTop && t > getTemp2(_x, _y + 1)) {
				storeChange(_x, _y, _x, _y + 1);
				return;
			} break;
		case 1:
			if (waterTopRight && t > getTemp2(_x + 1, _y + 1)) {
				storeChange(_x, _y, _x + 1, _y + 1);
				return;
			} break;
		case 2:
			if (waterTopLeft && t > getTemp2(_x - 1, _y + 1)) {
				storeChange(_x, _y, _x - 1, _y + 1);
				return;
			} break;
		
		}
		break;
	}
}


void Game::updateFire(int _x, int _y) {
	float t = m_particles[getIndex(_x, _y)].m_temp;
	float smooth;
	int fireHiStep = 1920;
	int fireLoStep = 1750;

	if (t >= 2000) {
		m_particles[getIndex(_x, _y)].m_color.x = 160;
		m_particles[getIndex(_x, _y)].m_color.y = 120;
		m_particles[getIndex(_x, _y)].m_color.z = 10;
	} else if (t > fireLoStep && t < fireHiStep) {
		smooth = (fireHiStep - fireLoStep) - ((t - fireHiStep) / (float)(fireHiStep - fireLoStep));
		m_particles[getIndex(_x, _y)].m_color.x = mymath::lerp(150, 180, t);
		m_particles[getIndex(_x, _y)].m_color.y = mymath::lerp(140, 40, t);
		m_particles[getIndex(_x, _y)].m_color.z = 10;
	} else if (t <= 1750) {
		m_particles[getIndex(_x, _y)].m_color.x = 180;
		m_particles[getIndex(_x, _y)].m_color.y = 40;
		m_particles[getIndex(_x, _y)].m_color.z = 10;
	}

	if (m_particles[getIndex(_x, _y)].m_temp <= FIRE_MINTEMP) {
		extinguishFire(_x, _y);
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

void Game::updateMetal(int _x, int _y) {
	float t = (m_particles[getIndex(_x, _y)].m_temp);
	float smooth;
	float metalLoStep = 800;
	float metalHiStep = 1200;
	float metalMeltingPoint = 1750;

	if (t <= 295) {
		m_particles[getIndex(_x, _y)].m_color.x = 70;
		m_particles[getIndex(_x, _y)].m_color.y = 70;
		m_particles[getIndex(_x, _y)].m_color.z = 70;
	} else if (t > 295 && t < metalLoStep) {
		smooth = t / metalLoStep;
		m_particles[getIndex(_x, _y)].m_color.x = mymath::lerp(70, 230, smooth);
		m_particles[getIndex(_x, _y)].m_color.y = 70;
		m_particles[getIndex(_x, _y)].m_color.z = 70;
	} else if (t >= metalLoStep && t < metalHiStep) {
		smooth = (t - metalLoStep) / (metalHiStep - metalLoStep);
		m_particles[getIndex(_x, _y)].m_color.x = 230;
		m_particles[getIndex(_x, _y)].m_color.y = mymath::lerp(70, 230, smooth);
		m_particles[getIndex(_x, _y)].m_color.z = mymath::lerp(70, 230, smooth);
	} else if (t >= metalHiStep) {
		m_particles[getIndex(_x, _y)].m_color.x = 230;
		m_particles[getIndex(_x, _y)].m_color.y = 230;
		m_particles[getIndex(_x, _y)].m_color.z = 230;
	}

	if (t < metalMeltingPoint) {
		return;
	}

	bool emptyBottom = isEmpty(_x, _y - 1);
	bool emptyBottomLeft = isEmpty(_x - 1, _y - 1);
	bool emptyBottomRight = isEmpty(_x + 1, _y - 1);

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

}

void Game::updateAcid(int _x, int _y) {
	bool topLeft = !isEmpty(_x - 1, _y + 1) && isAcid(_x - 1, _y + 1);
	bool top = !isEmpty(_x, _y + 1) && isAcid(_x, _y + 1);
	bool topRight = !isEmpty(_x + 1, _y + 1) && isAcid(_x + 1, _y + 1);
	bool left = !isEmpty(_x - 1, _y) && isAcid(_x - 1, _y);
	bool right = !isEmpty(_x + 1, _y) && isAcid(_x + 1, _y);
	bool bottom = !isEmpty(_x, _y - 1) && isAcid(_x, _y - 1);
	bool bottomLeft = !isEmpty(_x - 1, _y - 1) && isAcid(_x - 1, _y - 1);
	bool bottomRight = !isEmpty(_x + 1, _y - 1) && isAcid(_x + 1, _y - 1);

	int maxTemp = 1000;
	uint16_t t = m_particles[getIndex(_x, _y)].m_temp;
	if (!topLeft || !top || !topRight || !left || !right
		|| !bottom || !bottomLeft || bottomRight
	) {
		m_particles[getIndex(_x, _y)].m_temp = mymath::minu16(t + 15, 1000);
	}

	if (t >= 1000) {
		std::bitset<8> bits(0b00000000);
		if (topLeft) bits.set(0);
		if (top) bits.set(1);
		if (topRight) bits.set(2);
		if (left) bits.set(3);
		if (right) bits.set(4);
		if (bottomLeft) bits.set(5);
		if (bottom) bits.set(6);
		if (bottomRight) bits.set(7);
	}

	bool emptyLeft = isEmpty(_x - 1, _y);
	bool emptyRight = isEmpty(_x + 1, _y);
	bool emptyBottom = isEmpty(_x, _y - 1);
	bool emptyBottomLeft = isEmpty(_x - 1, _y - 1);
	bool emptyBottomRight = isEmpty(_x + 1, _y - 1);
	int choice = m_dist(m_rg) % 3;
	switch (choice) {
	case 0:
		if (emptyBottom) {
			storeChange(_x, _y, _x, _y - 1);
			return;
		} 
		break;
	case 1:
		if (emptyBottomLeft) {
			storeChange(_x, _y, _x - 1, _y - 1);
			return;
		} else if (emptyLeft) {
			storeChange(_x, _y, _x - 1, _y);
			return;
		}
		break;
	case 2:
		if (emptyBottomRight) {
			storeChange(_x, _y, _x + 1, _y - 1);
			return;
		} else if (emptyRight) {
			storeChange(_x, _y, _x + 1, _y);
			return;
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
			if((x + i) == 0 || (y + j) == 0 || (x + i) == TEXTURE_COLS - 1 || (y + j) == TEXTURE_ROWS - 1)
				continue;

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
			case METAL:
				metal(x + i, y + j);
				break;
			case TORCH:
				torch(x + i, y + j);
				break;
			case ACID:
				acid(x + i, y + j);
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

void Game::metal(int _x, int _y) {
	uint8_t c = static_cast<uint8_t>(m_dWall(m_rg));
	m_particles[getIndex(_x, _y)].m_id = METAL;
	m_particles[getIndex(_x, _y)].m_temp = METAL_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = METAL_COLOR[0];
	m_particles[getIndex(_x, _y)].m_color.y = METAL_COLOR[1];
	m_particles[getIndex(_x, _y)].m_color.z = METAL_COLOR[2];
}

void Game::acid(int _x, int _y) {
	m_particles[getIndex(_x, _y)].m_id = ACID;
	m_particles[getIndex(_x, _y)].m_temp = ACID_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = ACID_COLOR[0];
	m_particles[getIndex(_x, _y)].m_color.y = ACID_COLOR[1];
	m_particles[getIndex(_x, _y)].m_color.z = ACID_COLOR[2];
}

void Game::torch(int _x, int _y) {
	m_particles[getIndex(_x, _y)].m_id = TORCH;
	m_particles[getIndex(_x, _y)].m_temp = TORCH_BASETEMP;
	m_particles[getIndex(_x, _y)].m_color.x = 150;
	m_particles[getIndex(_x, _y)].m_color.y = 150;
	m_particles[getIndex(_x, _y)].m_color.z = 0;
}
