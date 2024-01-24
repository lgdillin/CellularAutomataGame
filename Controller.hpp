#pragma once

#include <iostream>

#include <GL/glew.h>
#include <SDL.h>

#include "Game.hpp"

class Controller {
public:
	Controller(Game &_game);
	~Controller();

	void update();

	void keyDown(SDL_KeyboardEvent *_event);
	void keyUp(SDL_KeyboardEvent *_event);

private:
	Game m_game;
};