#include "Controller.hpp"

Controller::Controller(Game &_game) {
	m_game = _game;
}

Controller::~Controller() {
}

void Controller::update() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
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
	}
}

void Controller::keyUp(SDL_KeyboardEvent *_event) {
	if (_event->repeat == 0) {
		if (_event->keysym.scancode == SDL_SCANCODE_W) {
			std::cout << "W released" << std::endl;
		}
	}
}
