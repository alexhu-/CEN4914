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
	GLuint bones[4];
	GLfloat weights[4];
	//int bonesFilled;
	//const unsigned int maxBones = 4;

	VertexData() :
		vertex(0.0f, 0.0f, 0.0f),
		normal(0.0f, 0.0f, 0.0f),
		textureCoordinate(0.0f, 0.0f),
		bones{0, 0, 0, 0},
		weights{0.0f, 0.0f, 0.0f, 0.0f}
		//bonesFilled(0)
	{

	}

	// adds a bone and associated weight to this struct
	// returns true if successful
	// returns false if there is no more space in the array
	/*bool addWeight(GLuint boneIndex, float weight)
	{
		if (bonesFilled >= maxBones)
		{
			return false;
		}
		bones[bonesFilled] = boneIndex;
		weights[bonesFilled] = weight;
		++bonesFilled;
		return true;
	}*/
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
	std::vector<GLuint> mIndices;
	std::vector<Texture> mTextures;
	std::vector<VertexData> mVertices;
	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;

	void setup();
};