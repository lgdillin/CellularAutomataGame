#pragma once

#include <GL/glew.h>
#include <SDL.h>

class View {
public:
	View();
	~View();

	void initialize();

	void repaint();

private:
	SDL_Window *m_window;
	SDL_GLContext m_glContext;
	SDL_Renderer *m_renderer;
};