#include <iostream>

#include <GL/glew.h>
#include <SDL.h>

#include "Game.hpp"
#include "View.hpp"

int main(int argc, char *argv[]) {

	Game game;
	//game.initialize();

	Controller controller(game);

	// init view
	View view(game, controller);
	view.initialize();

	// build all the objects
	game.initialize();

	// opengl init code goes here


	// main loop
	bool quit = false;
	SDL_Event e;

	while (!quit) {
		// update controller
		controller.update();

		// update game
		game.update();

		// update view
		view.repaint();
	}

	
	return 0;
}