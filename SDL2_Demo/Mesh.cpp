#include "Mesh.h"

Mesh::Mesh(std::vector<VertexData> vertices, std::vector<GLuint> indices, std::vector<Texture> textures) :
	mVertices(vertices),
	mIndices(indices),
	mTextures(textures)
{
	this->setup();
}

void Mesh::clearGLBuffers()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);
	glDeleteVertexArrays(1, &mVAO);
}

void Mesh::draw(GLint sampler)
{
	if (mTextures.size() > 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(sampler, 0);
		glBindTexture(GL_TEXTURE_2D, mTextures[0].id);
	}
	glBindVertexArray(this->mVAO);
	glDrawElements(GL_TRIANGLES, this->mIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	if (mTextures.size() > 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void Mesh::setup()
{
	glGenVertexArrays(1, &this->mVAO);
	glGenBuffers(1, &this->mVBO);
	glGenBuffers(1, &this->mEBO);

	glBindVertexArray(this->mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, this->mVBO);
	glBufferData(GL_ARRAY_BUFFER, this->mVertices.size() * sizeof(VertexData), &this->mVertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mIndices.size() * sizeof(GLuint), &this->mIndices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*) 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*)offsetof(VertexData, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (GLvoid*)offsetof(VertexData, textureCoordinate));


	glBindVertexArray(0);
}