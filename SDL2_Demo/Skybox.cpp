#include "Skybox.h"

Skybox::Skybox(std::vector<std::string> filenames) :
	mFileNames(filenames)
{
	float temp[] = 
	{     
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	/*unsigned int temp2[3 * 2 * 6] =
	{
		2, 1, 0,
		0, 3, 2,
		3, 0, 4,
		4, 7, 3,
		4, 5, 6,
		4, 7, 6,
		5, 6, 2,
		2, 1, 5,
		5, 1, 0,
		4, 5, 0,
		2, 6, 7,
		7, 3, 6
	};*/

	for (unsigned int i = 0; i != 108; ++i)
	{
		mCube[i] = temp[i];
	}
	/*
	for (unsigned int i = 0; i != 36; ++i)
	{
		mIndices[i] = temp2[i];
	}*/
}

void Skybox::setup()
{
	glGenVertexArrays(1, &this->mVAO);
	glGenBuffers(1, &this->mVBO);
	//glGenBuffers(1, &this->mEBO);

	glBindVertexArray(this->mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(this->mCube), &this->mCube, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mEBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->mIndices), &this->mIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);

	glBindVertexArray(0);

	loadCubemap(mFileNames);
}

void Skybox::cleanup()
{
	glDeleteBuffers(1, &mVBO);
	//glDeleteBuffers(1, &mEBO);
	glDeleteVertexArrays(1, &mVAO);
	glDeleteTextures(1, &mTextureId);
}

void Skybox::loadCubemap(std::vector<std::string> filenames)
{
	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);

	int width, height, channels;
	unsigned char* image;
	for (unsigned int i = 0; i < filenames.size(); ++i)
	{
		image = SOIL_load_image(filenames[i].c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::draw(GLuint sampler)
{
	glBindVertexArray(mVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(sampler, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);
	//glDrawElements(GL_TRIANGLES, 3 * 2 * 6, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}