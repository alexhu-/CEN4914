#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Model.h"

// Class to hold information about what to display onto the screen
class Scene
{
public:
	typedef void (*set_bone_transforms)(GLuint, GLuint, glm::mat4);

	Scene();
	~Scene();
	void addModel(Model* pModel);
	void render(GLuint shader, GLuint textureSampler, GLint modelUniform, set_bone_transforms setBoneTransformsMethod, double runningTime);

private:
	std::vector<Model*> mModels;
};