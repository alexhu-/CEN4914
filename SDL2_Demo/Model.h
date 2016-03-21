#pragma once

#include <iostream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "ModelData.h"

class Model
{
public:
	Model(
		std::vector<AnimationEntry> entries = std::vector<AnimationEntry>(),
		std::map<std::string, GLuint> boneMap = std::map<std::string, GLuint>(),
		std::vector<BoneData> bones = std::vector<BoneData>(), 
		std::vector<Mesh> meshes = std::vector<Mesh>(), 
		glm::mat4 globalInverseTransform = glm::mat4()
		);
	Model(const Model& rhs);
	~Model();
	void clearGLBuffers();
	void draw(GLint sampler);
	void draw(GLint sampler, GLint modelUniform, glm::mat4 originalModel);
	std::vector<glm::mat4> getBoneTransforms(float timeMS, unsigned int animationIndex);
	void calculateFinalTransforms(float animationTime, glm::mat4 parentTransform, const NodeData* node, unsigned int animationIndex);
	NodeData* getRoot();

	Model& operator=(const Model& rhs);

	// Call this to create the buffers for each mesh in opengl
	void setup();

	// Functions for manipulating the scale, translation, and rotation of the model
	void scale(float x, float y, float z);
	void translate(float x, float y, float z);
	void rotateX(float degrees);
	void rotateY(float degrees);
	void rotateZ(float degrees);

	void translateInWorld(float x, float y, float z);

	// Functions to get the model matrix
	glm::mat4 getModelMatrix();
	glm::mat4 getWorldModelMatrix();

	void printAnimationData();

private:
	// mesh information
	std::vector<BoneData> mBones;
	std::vector<Mesh> mMeshes;
	glm::mat4 mGlobalInverseTransform;

	// bone transfomation data
	std::vector<AnimationEntry> mAnimationEntries;
	unsigned int mBoneCount;
	NodeData* mRoot;
	std::map<std::string, GLuint> mBoneMap;

	// the model's model matrix - a combination of scaling, rotation, and translation
	glm::mat4 mModelMatrix;
	glm::mat4 mWorldModelMatrix;

	unsigned int findTimeIndex(float animationTime, const float* times, unsigned int timeCount);
	glm::vec3 getInterpolatedVector(float animationTime, const glm::vec3* vectors, const float* times, unsigned int size);
	glm::quat getInterpolatedRotation(float animationTime, const glm::quat* quaternions, const float* times, unsigned int size);
	void copyNodeTree(NodeData& out, const NodeData& in);
};