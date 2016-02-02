#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>



// Represents the keyframe data of the animation
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

	AnimationData(unsigned int positionCount = 0, unsigned int rotationCount = 0, unsigned int scalingCount = 0) :
		positionCount(positionCount),
		quaternionCount(rotationCount),
		scalingCount(scalingCount),
		positionTimes(nullptr),
		positionValues(nullptr),
		quaternionTimes(nullptr),
		quaternionValues(nullptr),
		scalingTimes(nullptr),
		scalingValues(nullptr)
	{
		if (positionCount > 0)
		{
			positionTimes = new float[positionCount];
			positionValues = new glm::vec3[positionCount];
		}

		if (quaternionCount > 0)
		{
			quaternionTimes = new float[quaternionCount];
			quaternionValues = new glm::quat[quaternionCount];
		}

		if (scalingCount > 0)
		{
			scalingTimes = new float[scalingCount];
			scalingValues = new glm::vec3[scalingCount];
		}
	}


	void copyFloatArray(float* src, unsigned int size, float* dest)
	{
		for (unsigned int i = 0; i < size; ++i)
		{
			dest[i] = src[i];
		}
	}

	void copyVec3Array(glm::vec3* src, unsigned int size, glm::vec3* dest)
	{
		for (unsigned int i = 0; i < size; ++i)
		{
			dest[i] = src[i];
		}
	}

	void copyQuatArray(glm::quat* src, unsigned int size, glm::quat* dest)
	{
		for (unsigned int i = 0; i < size; ++i)
		{
			dest[i] = src[i];
		}
	}

	void cleanup()
	{
		if (positionCount > 0)
		{
			delete[] positionTimes;
			delete[] positionValues;
		}

		if (quaternionCount > 0)
		{
			delete[] quaternionTimes;
			delete[] quaternionValues;
		}

		if (scalingCount > 0)
		{
			delete[] scalingTimes;
			delete[] scalingValues;
		}
	}

	~AnimationData()
	{
		cleanup();
	}

	AnimationData(const AnimationData& rhs)
	{
		this->positionCount = rhs.positionCount;
		this->quaternionCount = rhs.quaternionCount;
		this->scalingCount = rhs.scalingCount;

		if (this->positionCount > 0)
		{
			this->positionTimes = new float[positionCount];
			copyFloatArray(rhs.positionTimes, rhs.positionCount, this->positionTimes);
			this->positionValues = new glm::vec3[positionCount];
			copyVec3Array(rhs.positionValues, rhs.positionCount, this->positionValues);
		}

		if (this->quaternionCount > 0)
		{
			this->quaternionTimes = new float[quaternionCount];
			copyFloatArray(rhs.quaternionTimes, rhs.quaternionCount, this->quaternionTimes);
			this->quaternionValues = new glm::quat[quaternionCount];
			copyQuatArray(rhs.quaternionValues, rhs.quaternionCount, this->quaternionValues);
		}

		if (this->scalingCount > 0)
		{
			this->scalingTimes = new float[scalingCount];
			copyFloatArray(rhs.scalingTimes, rhs.scalingCount, this->scalingTimes);
			this->scalingValues = new glm::vec3[scalingCount];
			copyVec3Array(rhs.scalingValues, rhs.scalingCount, this->scalingValues);
		}
	}

	AnimationData& operator=(const AnimationData& rhs)
	{
		if (this != &rhs)
		{
			this->cleanup();

			this->positionCount = rhs.positionCount;
			this->quaternionCount = rhs.quaternionCount;
			this->scalingCount = rhs.scalingCount;

			if (this->positionCount > 0)
			{
				this->positionTimes = new float[positionCount];
				copyFloatArray(rhs.positionTimes, rhs.positionCount, this->positionTimes);
				this->positionValues = new glm::vec3[positionCount];
				copyVec3Array(rhs.positionValues, rhs.positionCount, this->positionValues);
			}

			if (this->quaternionCount > 0)
			{
				this->quaternionTimes = new float[quaternionCount];
				copyFloatArray(rhs.quaternionTimes, rhs.quaternionCount, this->quaternionTimes);
				this->quaternionValues = new glm::quat[quaternionCount];
				copyQuatArray(rhs.quaternionValues, rhs.quaternionCount, this->quaternionValues);
			}

			if (this->scalingCount > 0)
			{
				this->scalingTimes = new float[scalingCount];
				copyFloatArray(rhs.scalingTimes, rhs.scalingCount, this->scalingTimes);
				this->scalingValues = new glm::vec3[scalingCount];
				copyVec3Array(rhs.scalingValues, rhs.scalingCount, this->scalingValues);
			}
		}

		return *this;
	}
};



// Stores the ticks per second and duration of an animation
struct AnimationEntry
{
	float ticksPerSecond;
	float duration;

	AnimationEntry() :
		ticksPerSecond(0.0f),
		duration(0.0f)
	{

	}
};



// Stores id, bone offset, and final transformation of the bone
struct BoneData
{
	GLuint id;
	glm::mat4 offset;
	glm::mat4 finalTransformation;
};



// Stores data extracted from an aiNode
struct NodeData
{
	std::string name;
	glm::mat4 transformation;
	std::vector<NodeData*> children;
	std::vector<AnimationData> animations;

	NodeData() :
		name("")
	{

	}

	~NodeData()
	{
		cleanup();
	}

	void cleanup()
	{
		unsigned int size = children.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			if (children[i] != nullptr)
			{
				children[i]->cleanup();
				NodeData* child = children[i];
				children[i] = nullptr;
				delete child;
			}
		}
	}

	void addChild()
	{
		NodeData* child = new NodeData();
		this->children.push_back(child);
	}
};



// Stores the id, material index, and file name of a texture
struct Texture
{
	GLuint id;
	GLuint materialIndex;
	std::string fileName;
};



// Data structure for storing information about vertices
// Stores position, normal, and texture coordinate
// Stores bone ids and associated weights for animation
struct VertexData
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoordinate;
	GLuint boneIndices[4];
	float weights[4];

	VertexData() :
		position(0.0f, 0.0f, 0.0f),
		normal(0.0f, 0.0f, 0.0f),
		textureCoordinate(0.0f, 0.0f),
		boneIndices{ 0, 0, 0, 0 },
		weights{ 1.0f, 0.0f, 0.0f, 0.0f },
		bonesFilled(0)
	{

	}

	// Adds a bone and weight
	// Returns true when successful and false if arrays are full
	bool addWeight(GLuint boneIndex, float weight)
	{
		if (bonesFilled >= 4)
		{
			return false;
		}
		boneIndices[bonesFilled] = boneIndex;
		weights[bonesFilled] = weight;
		++bonesFilled;
		return true;
	}

private:
	// Keeps track of which filled indices
	unsigned int bonesFilled;
};
