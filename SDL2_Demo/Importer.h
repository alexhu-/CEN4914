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

#include "Mesh.h"
#include "Model.h"
#include "ModelData.h"

namespace Importer
{
	glm::mat4 glmMatrix(const aiMatrix4x4& matrix);

	glm::quat glmQuaternion(const aiQuaternion& quat);

	glm::vec3 glmVec3(const aiVector3D& vector);

	void copyNodes(NodeData* out, const aiNode* node);

	void loadBones(const aiMesh* mesh, std::vector<BoneData>* bones, std::map<std::string, GLuint>* boneMap, std::vector<VertexData>* vertices);

	GLuint loadTexture(std::string path, std::string directory);

	std::vector<Texture> loadMaterialTexture(aiMaterial* material, GLuint materialIndex, aiTextureType type, std::string directory);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene, const std::vector<std::vector<Texture>>& textures, std::vector<BoneData>* bones, std::map<std::string, GLuint>* boneMap);

	void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>* meshes, std::vector<std::vector<Texture>> textures, std::vector<BoneData>* bones, std::map<std::string, GLuint>* boneMap);

	Model loadModel(std::string path, std::string textureDirectory);

	std::vector<VertexData> processVertices(aiMesh* mesh);

	void processKeyframes(NodeData* out, const aiScene* scene);

	std::vector<std::vector<Texture>> processTextures(const aiScene* scene, std::string textureDirectory);

	std::vector<AnimationEntry> processAnimations(const aiScene* scene);

	void copyAnimationData(NodeData& out, const aiAnimation* animation);
}