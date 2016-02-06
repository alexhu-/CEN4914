#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

// Representation of a rectangle in opengl
// Always orthogonal to the camera's direction
// Always remain at a fixed distance away from the camera
class UIRectangle
{
public:
	UIRectangle(float x, float y, float w, float h, glm::vec4 rgba);
	void setup();
	void setX(float x);
	void setY(float y);
	void setZ(float z);
	void setWidth(float w);
	void setHeight(float h);
	void draw(GLuint colorUniform);
	void cleanup();

private:
	float mX;
	float mY;
	float mZ;
	float mWidth;
	float mHeight;
	glm::vec4 mRGBA;
	float mVertices[12];
	GLuint mIndices[6];
	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;

	bool mIsDataModified;

	const unsigned int sizeofvertices = 12;
	const unsigned int sizeofindices = 6;
};