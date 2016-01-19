#include "Model.h"

Model::Model(
	NodeData* root, 
	std::vector<AnimationEntry> entries,
	std::map<std::string, GLuint> boneMap,
	std::vector<BoneData> bones, 
	std::vector<Mesh> meshes, 
	glm::mat4 globalInverseTransform
	) :
	mRoot(root),
	mAnimationEntries(entries),
	mBoneMap(boneMap),
	mBones(bones),
	mMeshes(meshes),
	mGlobalInverseTransform(globalInverseTransform)
{
	mBoneCount = mBones.size();
}

Model::~Model()
{
	//delete mRoot;
	clearGLBuffers();
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

std::vector<glm::mat4> Model::getBoneTransforms(float timeMS, unsigned int animationIndex)
{
	glm::mat4 identity(1.0f);
	std::vector<glm::mat4> boneTransforms;
	boneTransforms.resize(mBoneCount);

	if (animationIndex >= mAnimationEntries.size())
	{
		for (unsigned int i = 0; i < mBoneCount; ++i)
		{
			boneTransforms[i] = glm::mat4(1.0f);
		}
		return boneTransforms;
	}

	float timeSeconds = timeMS / 1000.0f;
	float ticksPerSecond = mAnimationEntries[animationIndex].ticksPerSecond;
	float duration = mAnimationEntries[animationIndex].duration;
	float timeInTicks = timeSeconds * ticksPerSecond;
	float animationTime = std::fmod(timeInTicks, duration);

	calculateFinalTransforms(animationTime, identity, mRoot, animationIndex);

	for (unsigned int i = 0; i < mBoneCount; ++i)
	{
		boneTransforms[i] = mBones[i].finalTransformation;
	}

	return boneTransforms;
}

void Model::calculateFinalTransforms(float animationTime, glm::mat4 parentTransform, const NodeData* node, unsigned int animationIndex)
{
	glm::mat4 nodeTransform = node->transformation;

	// check if node has animations
	if ((node->animations.size() > 0) && (node->animations.size() > animationIndex))
	{
		AnimationData animationData = node->animations[animationIndex];

		// interpolate for all these matrices
		glm::vec3 scaling = getInterpolatedVector(animationTime, animationData.scalingValues, animationData.scalingTimes, animationData.scalingCount);
		glm::mat4 scalingTransform(1.0f);
		scalingTransform= glm::scale(scalingTransform, scaling);
		
		glm::quat rotation = getInterpolatedRotation(animationTime, animationData.quaternionValues, animationData.quaternionTimes, animationData.quaternionCount);
		glm::mat4 rotationTransform(rotation);

		glm::vec3 translation = getInterpolatedVector(animationTime, animationData.positionValues, animationData.positionTimes, animationData.positionCount);;
		glm::mat4 translationTransform(1.0f);
		translationTransform = glm::translate(translationTransform, translation);

		nodeTransform = translationTransform * rotationTransform * scalingTransform;
	}

	glm::mat4 globalTransform = parentTransform * nodeTransform;

	if (mBoneMap.find(node->name) != mBoneMap.end())
	{
		unsigned int boneIndex = mBoneMap[node->name];
		mBones[boneIndex].finalTransformation = mGlobalInverseTransform * globalTransform * mBones[boneIndex].offset;
	}

	for (unsigned int i = 0; i < node->childrenCount; ++i)
	{
		calculateFinalTransforms(animationTime, globalTransform, node->children[i], animationIndex);
	}
}

unsigned int Model::findTimeIndex(float animationTime, const float* times, unsigned int timeCount)
{
	// search for an index where the time is greater than animationTime
	// this is to get a time to interpolate to
	for (unsigned int i = 0; i < (timeCount - 1); ++i)
	{
		if (times[i + 1] > animationTime)
		{
			return i;
		}
	}

	// return 0 index if nothting to interpolate to
	return 0;
}

glm::vec3 Model::getInterpolatedVector(float animationTime, const glm::vec3* vectors, const float* times, unsigned int size)
{
	// nothing to interpolate to
	if (size == 1)
	{
		return vectors[0];
	}

	// find two values to interpolate between
	unsigned int index = findTimeIndex(animationTime, times, size);
	float deltaTime = times[index + 1] - times[index];
	float deltaAnimationTime = animationTime - times[index];

	// the percentage of how close is animationTime from the end time
	float factor = deltaAnimationTime / deltaTime;
	glm::vec3 start(vectors[index]);
	glm::vec3 end(vectors[index + 1]);
	glm::vec3 delta = end - start;
	return glm::vec3(start + factor * delta);
}

glm::quat Model::getInterpolatedRotation(float animationTime, const glm::quat* quaternions, const float* times, unsigned int size)
{
	// nothing to interpolate to
	if (size == 1)
	{
		return quaternions[0];
	}

	// find two values to interpolate between
	unsigned int index = findTimeIndex(animationTime, times, size);
	float deltaTime = times[index + 1] - times[index];
	float deltaAnimationTime = animationTime - times[index];

	// the percentage of how close is animationTime from the end time
	float factor = deltaAnimationTime / deltaTime;
	if (factor > 1.0f || factor < 0.0f)
	{
		return glm::quat(glm::mat4(1.0f));
	}
	glm::quat start(quaternions[index]);
	glm::quat end(quaternions[index + 1]);
	glm::quat result = glm::lerp(start, end, factor);
	return glm::normalize(result);
}