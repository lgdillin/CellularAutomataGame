#pragma once

#include "Game.hpp"

class Controller {
public:
	Controller(Game &_game);
	~Controller();

private:
	Game m_game;
};