#include "UIRectangle.h"

UIRectangle::UIRectangle(float x, float y, float w, float h, glm::vec4 rgba) :
	mX(x),
	mY(y),
	mWidth(w),
	mHeight(h),
	mRGBA(rgba),
	mIndices{0, 1, 2, 3, 0, 2},
	mIsDataModified(true)
{
	setX(mX);
	setY(mY);
	setZ(0.0f);
}

void UIRectangle::setup()
{
	glGenVertexArrays(1, &this->mVAO);
	glGenBuffers(1, &this->mVBO);
	glGenBuffers(1, &this->mEBO);

	glBindVertexArray(this->mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeofvertices * sizeof(float), &this->mVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeofindices * sizeof(GLuint), &this->mIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	mIsDataModified = false;
}

void UIRectangle::setX(float x)
{
	mX = x;

	mVertices[0] = mX;
	mVertices[3] = mX + mWidth;
	mVertices[6] = mX + mWidth;
	mVertices[9] = mX;

	mIsDataModified = true;
}

void UIRectangle::setY(float y)
{
	mY = y;

	mVertices[1] = mY;
	mVertices[4] = mY;
	mVertices[7] = mY - mHeight;
	mVertices[10] = mY - mHeight;

	mIsDataModified = true;
}

void UIRectangle::setZ(float z)
{
	mZ = z;

	mVertices[2] = mZ;
	mVertices[5] = mZ;
	mVertices[8] = mZ;
	mVertices[11] = mZ;

	mIsDataModified = true;
}

void UIRectangle::setWidth(float w)
{
	mWidth = w;

	setX(mX);
}

void UIRectangle::setHeight(float h)
{
	mHeight = h;

	setY(mY);
}

void UIRectangle::draw(GLuint colorUniform)
{
	glUniform4f(colorUniform, mRGBA[0], mRGBA[1], mRGBA[2], mRGBA[3]);
	glBindVertexArray(mVAO);

	if (mIsDataModified)
	{
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeofvertices * sizeof(float), &this->mVertices[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		mIsDataModified = false;
	}

	glDrawElements(GL_TRIANGLES, sizeofindices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void UIRectangle::cleanup()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);
	glDeleteVertexArrays(1, &mVAO);
}