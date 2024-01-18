#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Shader {

	static std::string loadShaderFile(std::string _file) {
		std::ifstream fileStream(_file, std::ios::in);

		if (!fileStream.is_open()) {
			std::cout << "File: " << _file << " couldn't open\n";
			throw;
		}

		std::string contents = "";
		std::string line = "";
		while (!fileStream.eof()) {
			std::getline(fileStream, line);
			contents.append(line + "\n");
		}
		fileStream.close();

		return contents;
	}

	static GLuint buildShaderStage(GLuint _program, GLenum _stage, std::string _shaderCode) {
		GLuint shader = glCreateShader(_stage);

		const GLchar *str = _shaderCode.c_str();
		const GLchar *const *code = &str;

		glShaderSource(shader, 1, code, nullptr);
		glCompileShader(shader);

		// compilation check
		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLchar log[1024] = { 0 };
			glGetShaderInfoLog(shader, 1024, nullptr, log);
			std::cerr << "Shader failed: " << log << std::endl;
			throw;
		}
		glAttachShader(_program, shader);

		return shader;
	}

	static GLuint buildShaderProgram(std::string _vFile, std::string _fFile) {
		GLuint program = glCreateProgram();

		std::string vertexCode = loadShaderFile(_vFile);
		GLuint vertexShader = buildShaderStage(program, GL_VERTEX_SHADER, vertexCode);

		std::string fragmentCode = loadShaderFile(_fFile);
		GLuint fragmentShader = buildShaderStage(program, GL_FRAGMENT_SHADER, fragmentCode);

		glLinkProgram(program);

		GLint result = 0;
		GLchar errorLog[1024] = { 0 };
		glGetProgramiv(program, GL_LINK_STATUS, &result);
		if (!result) {
			glGetProgramInfoLog(program, sizeof(errorLog), NULL, errorLog);
			std::cout << "Error Linking shader program " << errorLog << std::endl;
			throw;
		}

		return program;
	}


	static void setMat4(GLuint _shaderId, std::string _glslCode, glm::mat4 _mat4) {
		glUniformMatrix4fv(glGetUniformLocation(_shaderId, _glslCode.c_str()),
			1, GL_FALSE, glm::value_ptr(_mat4));
	}

	static void setTexture(GLuint _shaderId, std::string _glslCode, GLenum _textureUnit) {
		GLuint uniformLocation = glGetUniformLocation(_shaderId,
			_glslCode.c_str());
		glUniform1i(uniformLocation, _textureUnit);
	}

	static void setXY(GLuint _shaderId, std::string _glslCode, glm::vec2 _v) {
		GLuint uniformLocation = glGetUniformLocation(_shaderId,
			_glslCode.c_str());
		glUniform2fv(uniformLocation, 1, glm::value_ptr(_v));
	}
}