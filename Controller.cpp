#include "Controller.hpp"

Controller::Controller(Game &_game) : m_game(&_game) {
	m_particleChoice = 0;
}

Controller::~Controller() {
}

void Controller::update() {
	SDL_Event event;
	// I love you 
	// 
	Uint32 mouseState = SDL_GetMouseState(&m_game->m_mousex, &m_game->m_mousey);
	if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		m_game->brush();
	}

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_MOUSEMOTION:
			m_game->m_mousex = event.motion.x;
			m_game->m_mousey = event.motion.y;
		//case SDL_MOUSEBUTTONDOWN:
			//m_game->addParticle();
			//event.
		case SDL_KEYDOWN:
			keyDown(&event.key);
			break;

		case SDL_KEYUP:
			keyUp(&event.key);
			break;

		case SDL_QUIT:
			exit(0);
			break;

		default:
			break;
		}
	}
}

void Controller::keyDown(SDL_KeyboardEvent *_event) {
	if (_event->repeat == 0) {
		if (_event->keysym.scancode == SDL_SCANCODE_UP) {
			if (m_game->m_brushSize <= MAX_BRUSH_SIZE) {
				m_game->m_brushSize += 2;
			}
		}

		if (_event->keysym.scancode == SDL_SCANCODE_DOWN) {
			if (m_game->m_brushSize >= MIN_BRUSH_SIZE) {
				m_game->m_brushSize -= 2;
			}
		}

		if (_event->keysym.scancode == SDL_SCANCODE_1) {
			m_game->m_selectedParticle = WALL;
		}

		if (_event->keysym.scancode == SDL_SCANCODE_2) {
			m_game->m_selectedParticle = SAND;
		}

		if (_event->keysym.scancode == SDL_SCANCODE_3) {
			m_game->m_selectedParticle = WATER;
		}

		if (_event->keysym.scancode == SDL_SCANCODE_4) {
			m_game->m_selectedParticle = FIRE;
		}

		if (_event->keysym.scancode == SDL_SCANCODE_5) {
			m_game->m_selectedParticle = METAL;
		}

		if (_event->keysym.scancode == SDL_SCANCODE_6) {
			m_game->m_selectedParticle = TORCH;
		}
	}
}

void Controller::keyUp(SDL_KeyboardEvent *_event) {
	if (_event->repeat == 0) {
		if (_event->keysym.scancode == SDL_SCANCODE_W) {
			std::cout << "W released" << std::endl;
		}
	}
}
