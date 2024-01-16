#include <iostream>

#include <GL/glew.h>
#include <SDL.h>

int main(int argc, char *argv[]) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Log("Couldn't initialize SDL: Error %s\n", SDL_GetError());
		return -1;
	}

	SDL_Window *window = SDL_CreateWindow("SDL2 OpenGL", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
	if (!window) {
		SDL_Log("Window failed to create, error: %s\n", SDL_GetError());
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		SDL_Log("Error init Glew: %s\n", glewGetErrorString(glewError));
		return -1;
	}

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

		// rendering code here
		SDL_GL_SwapWindow(window);
	}

	// Cleanup
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	
	return 0;
}