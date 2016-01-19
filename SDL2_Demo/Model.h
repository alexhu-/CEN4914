#pragma once

#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Mesh.h"

// represents the keyframe data of the animation
struct AnimationData
{
	unsigned int positionCount;
	unsigned int quaternionCount;
	unsigned int scalingCount;
	float* positionTimes;
	glm::vec3* positionValues;
	float* quaternionTimes;
	glm::quat* quaternionValues;
	float* scalingTimes;
	glm::vec3* scalingValues;

	AnimationData(unsigned int positionCount, unsigned int rotationCount, unsigned int scalingCount) :
		positionCount(positionCount),
		quaternionCount(rotationCount),
		scalingCount(scalingCount),
		positionTimes(new float[positionCount]),
		positionValues(new glm::vec3[positionCount]),
		quaternionTimes(new float[rotationCount]),
		quaternionValues(new glm::quat[rotationCount]),
		scalingTimes(new float[scalingCount]),
		scalingValues(new glm::vec3[scalingCount])
	{

	}

	~AnimationData()
	{
		/*delete[] positionTimes;
		delete[] positionValues;
		delete[] quaternionTimes;
		delete[] quaternionValues;
		delete[] scalingTimes;
		delete[] scalingValues;*/
	}
};

// represents genral information about the animation
struct AnimationEntry
{
	float ticksPerSecond;
	float duration;
};

struct BoneData
{
	GLuint id;
	glm::mat4 offset;
	glm::mat4 finalTransformation;
};

// TODO: needs to manage its own data
struct NodeData
{
	std::string name;
	unsigned int childrenCount;
	unsigned int animationCount;
	glm::mat4 transformation;
	NodeData** children;
	std::vector<AnimationData> animations;

	NodeData() :
		name(""),
		childrenCount(0),
		animationCount(0),
		children(nullptr)
	{

	}

	~NodeData()
	{
		//cleanup();
	}

	void cleanup()
	{
		for (unsigned int i = 0; i < childrenCount; ++i)
		{
			children[i]->cleanup();
			delete children[i];
		}
		if (childrenCount > 0)
		{
			delete[] children;
		}
	}
};

class Model
{
public:
	Model(
		NodeData* root = nullptr,
		std::vector<AnimationEntry> entries = std::vector<AnimationEntry>(),
		std::map<std::string, GLuint> boneMap = std::map<std::string, GLuint>(),
		std::vector<BoneData> bones = std::vector<BoneData>(), 
		std::vector<Mesh> meshes = std::vector<Mesh>(), 
		glm::mat4 globalInverseTransform = glm::mat4()
		);
	~Model();
	void clearGLBuffers();
	void draw(GLint sampler);
	std::vector<glm::mat4> getBoneTransforms(float timeMS, unsigned int animationIndex);
	void calculateFinalTransforms(float animationTime, glm::mat4 parentTransform, const NodeData* node, unsigned int animationIndex);

private:
	std::vector<BoneData> mBones;
	std::vector<Mesh> mMeshes;
	glm::mat4 mGlobalInverseTransform;

	// bone transfomation data
	std::vector<AnimationEntry> mAnimationEntries;
	unsigned int mBoneCount;
	NodeData* mRoot;
	std::map<std::string, GLuint> mBoneMap;

	unsigned int findTimeIndex(float animationTime, const float* times, unsigned int timeCount);
	glm::vec3 getInterpolatedVector(float animationTime, const glm::vec3* vectors, const float* times, unsigned int size);
	glm::quat getInterpolatedRotation(float animationTime, const glm::quat* quaternions, const float* times, unsigned int size);
};