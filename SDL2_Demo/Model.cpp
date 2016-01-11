#include "Model.h"

Model::Model(std::vector<Mesh> meshes) :
	mMeshes(meshes)
{

}

void Model::clearGLBuffers()
{
	for (GLuint i = 0; i < mMeshes.size(); ++i)
	{
		mMeshes[i].clearGLBuffers();
	}
}

void Model::draw(GLint sampler)
{
	for (GLuint i = 0; i < mMeshes.size(); ++i)
	{
		mMeshes[i].draw(sampler);
	}
}