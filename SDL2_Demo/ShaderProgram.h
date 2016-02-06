#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <GL/glew.h>

class ShaderProgram
{
public:
	ShaderProgram(std::string vertexShaderFile, std::string fragmentShaderFile);
	void compileShaderProgram();
	void use();
	GLuint getProgram();
	void cleanup();

private:
	void compileShader(std::string fileName, std::string type, GLuint shader);

	std::string mVertexShaderFile;
	std::string mFragmentShaderFile;
	//bool mSuccess; // bool if shader program compiled correctly
	GLuint mShaderProgram;
};