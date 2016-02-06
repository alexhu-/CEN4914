#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm\glm.hpp>

#include "ModelData.h"

class Mesh
{
public:
	Mesh(std::vector<VertexData> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	void clearGLBuffers();
	void draw(GLint sampler);

	void setup();

private:
	std::vector<GLuint> mIndices;
	std::vector<Texture> mTextures;
	std::vector<VertexData> mVertices;
	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;
};