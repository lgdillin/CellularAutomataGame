#include <iostream>

#include <GL/glew.h>
#include <SDL.h>

#include "Game.hpp"
#include "View.hpp"

int main(int argc, char *argv[]) {

	Game game;
	//game.initialize();

	// init view
	View view;
	view.initialize();

	// opengl init code goes here

	// main loop
	bool quit = false;
	SDL_Event e;

	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		// update view
		view.repaint();
	}

	
	return 0;
}