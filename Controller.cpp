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
		m_game->addParticle();
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
		if (_event->keysym.scancode == SDL_SCANCODE_W) {
			std::cout << "W pressed" << std::endl;
		}

		if (_event->keysym.scancode == SDL_SCANCODE_1) {
			m_game->m_particleChoice = WALL;
		}

		if (_event->keysym.scancode == SDL_SCANCODE_2) {
			m_game->m_particleChoice = SAND;
		}

		if (_event->keysym.scancode == SDL_SCANCODE_3) {
			m_game->m_particleChoice = WATER;
		}

		if (_event->keysym.scancode == SDL_SCANCODE_4) {
			m_game->m_particleChoice = FIRE;
		}

		if (_event->keysym.scancode == SDL_SCANCODE_5) {
			m_game->m_particleChoice = METAL;
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
