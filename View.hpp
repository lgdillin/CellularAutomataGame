#pragma once

#include <vector>

#include <GL/glew.h>
#include <SDL.h>

#include "Macros.hpp"

#include "Game.hpp"
#include "Controller.hpp"
#include "Shader.hpp"

#include "Particle.hpp"

class View {
public:
	View(Game &_game, Controller &_controller);
	~View();

	void repaint();

	void updateTexture();

	void initialize();

	SDL_Window *getWindow() { return m_mainWindow; }

	//std::uint8_t *m_particleColors;
	std::vector<GLubyte> m_pColors;
private:
	int m_mainWidth;
	int m_mainHeight;
	SDL_Window *m_mainWindow;
	SDL_Renderer *m_mainRenderer;
	SDL_GLContext m_glContext;

	Game *m_game;
	Controller *m_controller;

	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ibo;
	GLuint m_shaderId;
	GLuint m_textureId;
	GLuint m_framebuffer;

	//std::vector<std::vector<Particle>> m_pixels;
	//GLubyte data[100][100][4];

	GLfloat m_vertices[16] = {
		-1.0f, -1.0f, // pos
		0.0f, 0.0f, // texcoord

		1.0f, -1.0f,
		1.0f, 0.0f,

		-1.0f, 1.0f,
		0.0f, 1.0f,

		1.0f, 1.0f,
		1.0f, 1.0f
	};

	unsigned int m_indices[6] = {
		0, 2, 3,
		3, 1, 0
	};
};