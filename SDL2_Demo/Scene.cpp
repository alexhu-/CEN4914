#include "Scene.h"

Scene::Scene() :
	mModels(std::vector<Model*>())
{

}

Scene::~Scene()
{

}

void Scene::addModel(Model* pModel)
{
	mModels.push_back(pModel);
}

void Scene::render(GLuint shader, GLuint textureSampler, GLint modelUniform, set_bone_transforms setBoneTransformsMethod, double runningTime)
{
	set_bone_transforms boneTransformsMethod = setBoneTransformsMethod;
	for (unsigned int i = 0; i < mModels.size(); ++i)
	{
		Model* pModel = mModels[i];
		if (pModel != nullptr)
		{
			std::vector<glm::mat4> boneTransforms = pModel->getBoneTransforms((float)runningTime, 0);

			for (unsigned int i = 0; i < boneTransforms.size(); ++i)
			{
				if (i < 100)
				{
					(*boneTransformsMethod)(shader, i, boneTransforms[i]);
				}
			}

			pModel->draw(textureSampler, modelUniform, glm::mat4(1.0f));
		}
	}
}