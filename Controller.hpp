#pragma once

#include <iostream>

#include <GL/glew.h>
#include <SDL.h>

#include "Game.hpp"

class Controller {
public:
	Controller(Game &_game);
	~Controller();

	void initialize() { }

	void update();

	//void mouseDown(SDL_)
	void keyDown(SDL_KeyboardEvent *_event);
	void keyUp(SDL_KeyboardEvent *_event);

	bool m_visualMode;
private:
	int m_particleChoice;

	Game *m_game;
};