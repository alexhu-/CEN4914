#pragma once

#include <vector>

#include <GL/glew.h>

#include "Mesh.h"

class Model
{
public:
	Model(std::vector<Mesh> meshes = std::vector<Mesh>());
	void clearGLBuffers();
	void draw(GLint sampler);

private:
	std::vector<Mesh> mMeshes;
};