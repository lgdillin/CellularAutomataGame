#include <iostream>

#include <GL/glew.h>
#include <SDL.h>

#include "Game.hpp"
#include "View.hpp"

/// <summary>
/// I love you <3 J
/// </summary>
/// <param name="argc"></param>
/// <param name="argv"></param>
/// <returns></returns>
int main(int argc, char *argv[]) {

	Game game;

	Controller controller(game);

	// init view
	View view(game, controller);
	view.initialize();

	// build all the objects
	game.initialize();
	controller.initialize();

	// wait for OS to catch up
	SDL_Delay(1000);

	// main loop
	bool quit = false;
	SDL_Event e;
	const int targetFramerate = 40;
	const int frameDelay = 1000 / targetFramerate;
	Uint32 frameStart, frameTime;
	while (!quit) {
		frameStart = SDL_GetTicks();

		// update controller
		controller.update();

		// update game
		game.update();

		// update view
		view.repaint();

		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}

	
	return 0;
}