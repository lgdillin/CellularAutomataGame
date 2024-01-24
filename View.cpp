#include "View.hpp"

View::View(Game &_game, Controller &_controller) : m_controller(_game) {
	m_game = _game;
	m_controller = _controller;

	m_mainWidth = 800;
	m_mainHeight = 600;

	m_mainWindow = nullptr;
	m_glContext = nullptr;
	m_mainRenderer = nullptr;

	for (int i = 0; i < 100; ++i) {
		for (int j = 0; j < 100; ++j) {
			data[i][j][0] = 0;
			data[i][j][1] = 0;
			data[i][j][2] = 0;
			data[i][j][3] = 255;
		}
	}
}

View::~View() {
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_mainWindow);
	SDL_Quit();
}

void View::repaint() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	//glUseProgram(m_shaderId);

	for (auto &p : m_game.m_particles) {
		data[p.m_position.x][p.m_position.y][0] = p.m_color.x;
		data[p.m_position.x][p.m_position.y][1] = p.m_color.y;
		data[p.m_position.x][p.m_position.y][2] = p.m_color.z;
		data[p.m_position.x][p.m_position.y][3] = 255;
	}

	// render here
	Shader::setTexture(m_shaderId, "u_tex", 0);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	glUseProgram(m_shaderId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	SDL_GL_SwapWindow(m_mainWindow);
}

void View::initialize() {
	int rendererFlag = SDL_RENDERER_ACCELERATED;
	int windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		SDL_Log("Couldn't initialize SDL: Error %s\n", SDL_GetError());
		return;
	}

	// main window
	m_mainWindow = SDL_CreateWindow("SDL2 OpenGL", 
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED, 
		m_mainWidth, m_mainHeight, windowFlags);

	if (!m_mainWindow) {
		SDL_Log("Window failed to create, error: %s\n", SDL_GetError());
	}
	m_mainRenderer = SDL_CreateRenderer(m_mainWindow, -1, rendererFlag);

	m_glContext = SDL_GL_CreateContext(m_mainWindow);
	const GLubyte *version = glGetString(GL_VERSION);
	SDL_Log("OpenGL version %s", version);
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		SDL_Log("Error init Glew: %s\n", glewGetErrorString(glewError));
		return;
	}

	//
	//
	// Vertex buffers
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// create the data store for the indices
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int) * 6, m_indices, GL_STATIC_DRAW);

	// create the data store for the vertices
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(GLfloat) * 16, m_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), (GLvoid *)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_shaderId = Shader::buildShaderProgram("shader1.vert", "shader1.frag");

	// 
	//
	// texture building
	glGenTextures(1, &m_textureId);
	glBindTexture(GL_TEXTURE_2D, m_textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 100, 100, 
		0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);

	//
	//
	// generate framebuffer
	glGenFramebuffers(1, &m_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		m_textureId, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "framebuffer create failed" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
