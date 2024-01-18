#include "View.hpp"

View::View() {
	m_window = nullptr;
	m_glContext = nullptr;
}

View::~View() {
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void View::initialize() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Log("Couldn't initialize SDL: Error %s\n", SDL_GetError());
		return;
	}

	m_window = SDL_CreateWindow("SDL2 OpenGL", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
	if (!m_window) {
		SDL_Log("Window failed to create, error: %s\n", SDL_GetError());
	}

	// maybe need this?
	m_glContext = SDL_GL_CreateContext(m_window);
	// or this instead?
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	const GLubyte *version = glGetString(GL_VERSION);
	SDL_Log("OpenGL version %s", version);

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		SDL_Log("Error init Glew: %s\n", glewGetErrorString(glewError));
		return;
	}
}

void View::repaint() {
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);

	// render game objects

	SDL_GL_SwapWindow(m_window);
}
