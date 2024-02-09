#include <iostream>

#include <GL/glew.h>
#include <SDL.h>
#include <chrono>

#include "Game.hpp"
#include "View.hpp"

std::chrono::steady_clock::time_point frameStart, frameEnd;
int frameCount = 0;

void calculateFPS(View &_view) {
	frameEnd = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();

	if (duration >= 1000) {
		int fps = frameCount * 1000 / duration;
		//std::cout << "FPS: " << fps << std::endl;

		std::string title = "FPS Counter - FPS: " + std::to_string(fps);
		SDL_SetWindowTitle(_view.getWindow(), title.c_str());

		frameStart = std::chrono::steady_clock::now();
		frameCount = 0;
	} else {
		frameCount++;
	}
}

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
	//SDL_Delay(1000);

	// main loop
	bool quit = false;
	const int targetFramerate = 40;
	const int frameDelay = 1000 / targetFramerate;
	Uint32 frameStart, frameTime;
	while (!quit) {
		frameStart = SDL_GetTicks();

		// update controller
		controller.update();

		// update game
		//game.update();
		game.update2();

		// update view
		view.repaint();

		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
		calculateFPS(view);
	}

	
	return 0;
}