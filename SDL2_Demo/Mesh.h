#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm\glm.hpp>

struct VertexData
{
	// data structure for the vertex position
	// its associated normal and texture coordinate
	glm::vec3 vertex;
	glm::vec3 normal;
	glm::vec2 textureCoordinate;
};

struct Texture
{
	GLuint id;
	GLuint materialIndex;
	std::string fileName;
};

class Mesh
{
public:
	Mesh(std::vector<VertexData> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	void clearGLBuffers();
	void draw(GLint sampler);

private:
	std::vector<VertexData> mVertices;
	std::vector<GLuint> mIndices;
	std::vector<Texture> mTextures;
	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;

	void setup();
};