#include "View.hpp"

View::View(Game &_game, Controller &_controller) 
	: m_game(&_game), m_controller(&_controller)
{

	m_mainWidth = SCREEN_WIDTH;
	m_mainHeight = SCREEN_HEIGHT;
	m_pColors = std::vector<GLubyte>(TEXTURE_ROWS * TEXTURE_COLS * 3);

	m_mainWindow = nullptr;
	m_glContext = nullptr;
	m_mainRenderer = nullptr;
	m_vao = 0;
	m_vbo = 0;
	m_ibo = 0;
	m_shaderId = 0;
	m_textureId = 0;
	m_framebuffer = 0;
}

View::~View() {
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_mainWindow);
	SDL_Quit();
}

void View::repaint() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	updateTexture();
	

	glUseProgram(m_shaderId);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
	Shader::setTexture(m_shaderId, "u_tex", 0);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	SDL_GL_SwapWindow(m_mainWindow);
}

void View::updateTexture() {
	for (int i = 0; i < TEXTURE_ROWS * TEXTURE_COLS; ++i) {
		if (m_game->m_particles[i].m_id == EMPTY) {
			m_pColors[3 * i] = 0;
			m_pColors[3 * i + 1] = 0;
			m_pColors[3 * i + 2] = 0;
		} else {
			m_pColors[3 * i] = m_game->m_particles[i].m_color.x;
			m_pColors[3 * i + 1] = m_game->m_particles[i].m_color.y;
			m_pColors[3 * i + 2] = m_game->m_particles[i].m_color.z;
		}
	}

	glBindTexture(GL_TEXTURE_2D, m_textureId);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, TEXTURE_COLS, TEXTURE_ROWS, GL_RGB,
		GL_UNSIGNED_BYTE, m_pColors.data());
	glBindTexture(GL_TEXTURE_2D, 0);
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

	// allows for more frequent mouse polling (dont work)
	//SDL_SetRelativeMouseMode(SDL_TRUE);

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for (int i = 0; i < TEXTURE_ROWS * TEXTURE_COLS; ++i) {
		Particle p = m_game->m_particles[i];
		m_pColors[3 * i] = m_game->m_particles[i].m_color.x;
		m_pColors[3 * i + 1] = m_game->m_particles[i].m_color.y;
		m_pColors[3 * i + 2] = m_game->m_particles[i].m_color.z;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_COLS, TEXTURE_ROWS, 
		0, GL_RGB, GL_UNSIGNED_BYTE, m_pColors.data());

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
