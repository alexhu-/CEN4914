#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <SOIL.h>

#include "ShaderProgram.h"

class Skybox
{
public:
	Skybox(std::vector<std::string> filenames);
	void setup();
	void cleanup();
	void draw(GLuint sampler);

private:
	void loadCubemap(std::vector<std::string> filenames);

	GLuint mVAO;
	GLuint mVBO;
	//GLuint mEBO;
	GLuint mTextureId;
	std::vector<std::string> mFileNames;
	//float mCube[3 * 8]; // 3 floats per vertex * 8 vertices per cube
	//unsigned int mIndices[3 * 2 * 6]; // 3 points per triangle * 2 triangles per face * 6 faces per cube
	float mCube[108];
};