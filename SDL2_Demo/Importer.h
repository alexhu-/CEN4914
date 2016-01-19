#pragma once

#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <SOIL.h>

#include "Animation.h"
#include "Mesh.h"
#include "Model.h"

glm::mat4 glmMatrix(const aiMatrix4x4& in)
{
	/*for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			out[i][j] = in->;
		}
	}*/
	glm::mat4 out(1.0f);
	out[0][0] = in.a1;
	out[0][1] = in.b1;
	out[0][2] = in.c1;
	out[0][3] = in.d1;
	out[1][0] = in.a2;
	out[1][1] = in.b2;
	out[1][2] = in.c2;
	out[1][3] = in.d2;
	out[2][0] = in.a3;
	out[2][1] = in.b3;
	out[2][2] = in.c3;
	out[2][3] = in.d3;
	out[3][0] = in.a4;
	out[3][1] = in.b4;
	out[3][2] = in.c4;
	out[3][3] = in.d4;

	return out;
}

// converts aiNode to Node Data - a data structure defined in Model.h
// creates new NodeData when copying the tree
void copyNodes(NodeData* out, const aiNode* ainode)
{
	unsigned int childrenCount = ainode->mNumChildren;
	out->name = ainode->mName.C_Str();
	out->childrenCount = childrenCount;
	if (childrenCount > 0)
	{
		out->children = new NodeData*[childrenCount];
	}
	out->transformation = glmMatrix(ainode->mTransformation);

	for (unsigned int i = 0; i < childrenCount; ++i)
	{
		NodeData* child = new NodeData();
		out->children[i] = child;
		copyNodes(child, ainode->mChildren[i]);
	}
}

// convert assimp's quaternion to glm quaternion
glm::quat glmQuaternion(const aiQuaternion& aiquat)
{
	float x = aiquat.x;
	float y = aiquat.y;
	float z = aiquat.z;
	float w = aiquat.w;
	return glm::quat(w, x, y, z);
}

// convert assimp's vector3 to glm vec3
glm::vec3 glmVec3(const aiVector3D& aivector)
{
	float x = aivector.x;
	float y = aivector.y;
	float z = aivector.z;
	return glm::vec3(x, y, z);
}

// copy aiAnimation data to a node given animation index
// given node data, finds aiNodeAnim with same node name
// copies data from aiNodeAnim to node given animation index
void copyAnimationDataHelper(NodeData& out, const aiAnimation* aianimation, unsigned int animationIndex)
{
	for (unsigned int i = 0; i < aianimation->mNumChannels; ++i)
	{
		const aiNodeAnim* nodeAnim = aianimation->mChannels[i];

		if (out.name == std::string(nodeAnim->mNodeName.data))
		{
			unsigned int positionCount = nodeAnim->mNumPositionKeys;
			unsigned int rotationCount = nodeAnim->mNumRotationKeys;
			unsigned int scalingCount = nodeAnim->mNumScalingKeys;
			AnimationData animationData(positionCount, rotationCount, scalingCount);

			for (unsigned int j = 0; j < positionCount; ++j)
			{
				animationData.positionTimes[j] = nodeAnim->mPositionKeys[j].mTime;
				animationData.positionValues[j] = glmVec3(nodeAnim->mPositionKeys[j].mValue);
			}

			for (unsigned int j = 0; j < rotationCount; ++j)
			{
				animationData.quaternionTimes[j] = nodeAnim->mRotationKeys[j].mTime;
				animationData.quaternionValues[j] = glmQuaternion(nodeAnim->mRotationKeys[j].mValue);
			}

			for (unsigned int j = 0; j < scalingCount; ++j)
			{
				animationData.scalingTimes[j] = nodeAnim->mScalingKeys[j].mTime;
				animationData.scalingValues[j] = glmVec3(nodeAnim->mScalingKeys[j].mValue);
			}
			
			out.animations.push_back(animationData);
			break;
		}
	}
}

// recursively copies aiAnimation data to the nodes
// check if aiScene has animation first before using
void copyAnimationData(NodeData& out, const aiScene* aiscene)
{
	// initialize the node's animation array if it is not initialized
	unsigned int animationCount = aiscene->mNumAnimations;
	if (out.animations.size() <= 0)
	{
		out.animationCount = animationCount;
	}

	for (unsigned int i = 0; i < animationCount; ++i)
	{
		const aiAnimation* animation = aiscene->mAnimations[i];
		copyAnimationDataHelper(out, animation, i);
	}

	for (unsigned int i = 0; i < out.childrenCount; ++i)
	{
		copyAnimationData(*out.children[i], aiscene);
	}
}

void addWeight(VertexData& out, GLuint boneIndex, float weight)
{
	for (unsigned int i = 0; i < 4; ++i)
	{
		if (out.weights[i] == 0)
		{
			out.bones[i] = boneIndex;
			out.weights[i] = weight;
		}
	}
}

void loadBones(const aiMesh* aimesh, std::vector<BoneData>* bones, std::map<std::string, GLuint>* boneMap, std::vector<VertexData>* vertices)
{
	for (unsigned int i = 0; i < aimesh->mNumBones; ++i)
	{
		GLuint boneIndex = boneMap->size();
		std::string boneName(aimesh->mBones[i]->mName.data);

		if (boneMap->find(boneName) == boneMap->end())
		{
			BoneData boneData;
			boneData.id = boneIndex;
			boneData.offset = glmMatrix(aimesh->mBones[i]->mOffsetMatrix);
			bones->push_back(boneData);
			boneMap->insert(std::pair<std::string, GLuint>(boneName, boneData.id));
		}
		else
		{
			boneIndex = boneMap->at(boneName);
		}

		for (unsigned int j = 0; j < aimesh->mBones[i]->mNumWeights; ++j)
		{
			unsigned int vertexIndex = aimesh->mBones[i]->mWeights[j].mVertexId;
			float weight = aimesh->mBones[i]->mWeights[j].mWeight;
			addWeight(vertices->at(vertexIndex), boneIndex, weight);// addWeight(boneIndex, weight);
		}
	}
}

GLuint loadTexture(std::string path, std::string directory)
{
	std::string fileName = directory + "/" + path;
	GLuint textureId;
	glGenTextures(1, &textureId);

	int width;
	int height;
	int channel = 0;
	unsigned char* image = SOIL_load_image(fileName.c_str(), &width, &height, &channel, SOIL_LOAD_RGB);

	if (!image)
	{
		std::string cleanPath = path.substr(path.find_last_of("\\") + 1);
		fileName = directory + '/' + cleanPath;
		image = SOIL_load_image(fileName.c_str(), &width, &height, &channel, SOIL_LOAD_RGB);
	}

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// parameters for filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);

	return textureId;
}

std::vector<Texture> loadMaterialTexture(aiMaterial* material, GLuint materialIndex, aiTextureType type, std::string directory, Animation *animation)
{
	std::vector<Texture> textures;
	for (GLuint i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString texturePath;
		material->GetTexture(type, i, &texturePath);
		std::string textureFileName = texturePath.C_Str();
		Texture texture;
		if (animation && animation->isTextureLoaded(textureFileName))
		{
			Texture* texturePointer = animation->getTexture(textureFileName);
			if (texturePointer)
			{
				texture = *texturePointer;
			}
		}
		else
		{
			texture.id = loadTexture(textureFileName, directory);
			texture.materialIndex = materialIndex;
			texture.fileName = textureFileName;
			if (animation)
			{
				animation->addTexture(texture);
			}
		}
		textures.push_back(texture);
	}

	return textures;
}

Mesh processMesh(aiMesh* mesh, const aiScene* scene, const std::vector<std::vector<Texture>>& textures, std::vector<BoneData>* bones, std::map<std::string, GLuint>* boneMap)
{
	std::vector<VertexData> vertices;
	std::vector<GLuint> indices;
	vertices.resize(mesh->mNumVertices);

	for (GLuint i = 0; i < mesh->mNumVertices; ++i)
	{
		VertexData vertexData;
		glm::vec3 vector(0.0f, 0.0f, 0.0f);

		// the vertex of the mesh
		if (mesh->HasPositions())
		{
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
		}
		vertexData.vertex = vector;

		// the normal of the mesh
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
		}
		vertexData.normal = vector;

		// the texture coordinate of the mesh
		glm::vec2 textureCoord(0.0f, 0.0f);

		// check if there are texture coordinates in the mesh
		// if there are, we use them
		if (mesh->mTextureCoords[0])
		{
			// vertex can have up to 8 different texture coordinates... we will just use first set
			textureCoord.x = mesh->mTextureCoords[0][i].x;
			textureCoord.y = mesh->mTextureCoords[0][i].y;
		}
		vertexData.textureCoordinate = textureCoord;

		vertices[i] = vertexData;
	}

	// go through each of the mesh's faces and retrieve the corresponding vertex indicies
	// similar to element buffer
	for (GLuint i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];

		for (GLuint j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	std::vector<Texture> meshTextures;
	if (mesh->mMaterialIndex >= 0)
	{
		unsigned int materialIndex = mesh->mMaterialIndex;
		aiMaterial* material = scene->mMaterials[materialIndex];
		GLuint textureCount = material->GetTextureCount(aiTextureType_DIFFUSE);
		meshTextures.resize(textureCount);
		for (GLuint i = 0; i < textureCount; ++i)
		{
			meshTextures[i] = (textures[materialIndex][i]);
		}
	}

	loadBones(mesh, bones, boneMap, &vertices);

	return Mesh(vertices, indices, meshTextures);
}

void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>* meshes, std::vector<std::vector<Texture>> textures, std::vector<BoneData>* bones, std::map<std::string, GLuint>* boneMap)
{
	for (GLuint i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes->push_back(processMesh(mesh, scene, textures, bones, boneMap));
	}

	for (GLuint i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene, meshes, textures, bones, boneMap);
	}
}

Model loadModel(std::string path, std::string textureDirectory, Animation* animation = nullptr)
{
	// Assimp importer only imports one scene per instance of importer
	// importing another scene will clear the data of that importer
	// TODO: better comments?
	Assimp::Importer importer;
	glm::mat4 globalInverseTransform;
	std::map<std::string, GLuint> boneMap;

	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR: ASSIMP: " << importer.GetErrorString() << std::endl;
		return Model();
	}
	globalInverseTransform = glmMatrix(scene->mRootNode->mTransformation);
	globalInverseTransform = glm::inverse(globalInverseTransform);
	
	// process material
	// TODO: add some better comments
	std::vector<std::vector<Texture>> textures;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];
		textures.push_back(loadMaterialTexture(material, i, aiTextureType_DIFFUSE, textureDirectory, animation));
	}

	std::vector<AnimationEntry> animationEntries;
	for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
	{
		AnimationEntry entry;
		entry.duration = scene->mAnimations[i]->mDuration;
		entry.ticksPerSecond = scene->mAnimations[i]->mTicksPerSecond;
		animationEntries.push_back(entry);
	}

	std::vector<BoneData> bones;
	std::vector<Mesh> meshes;
	processNode(scene->mRootNode, scene, &meshes, textures, &bones, &boneMap);

	NodeData* root = new NodeData();
	copyNodes(root, scene->mRootNode);
	if (scene->HasAnimations())
	{
		copyAnimationData(*root, scene);
	}

	return Model(root, animationEntries, boneMap, bones, meshes, globalInverseTransform);
}

Animation loadAnimation(std::string path, std::vector<std::string> modelNames, std::string textureDirectory)
{
	Animation animation;

	std::clock_t overallStart = clock();

	for (GLuint i = 0; i < modelNames.size(); ++i)
	{
		std::clock_t begin = clock();

		std::string dest = path + modelNames[i];
		Model model = loadModel(dest, textureDirectory, &animation);
		animation.addModel(model);
		std::cout << "Loaded model " << i << std::endl;

		std::clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
		std::cout << elapsed_secs << " seconds have passed" << std::endl;
	}

	std::clock_t overallEnd = clock();
	std::cout << (overallEnd - overallStart) / CLOCKS_PER_SEC << " seconds have passed overall" << std::endl;

	return animation;
}