#pragma once

#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SOIL.h>

#include "Animation.h"
#include "Mesh.h"
#include "Model.h"

GLuint loadTexture(std::string path, std::string directory)
{
	std::string fileName = directory + "/" + path;
	GLuint textureId;
	glGenTextures(1, &textureId);

	int width;
	int height;
	int channel = 0;
	unsigned char* image = SOIL_load_image(fileName.c_str(), &width, &height, &channel, SOIL_LOAD_RGB);

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

Mesh processMesh(aiMesh* mesh, const aiScene* scene, std::vector<std::vector<Texture>> textures)
{
	std::vector<VertexData> vertices;
	std::vector<GLuint> indices;

	for (GLuint i = 0; i < mesh->mNumVertices; ++i)
	{
		VertexData vertexData;
		glm::vec3 vector;

		// the vertex of the mesh
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertexData.vertex = vector;

		// the normal of the mesh
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
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

		vertices.push_back(vertexData);
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
		for (GLuint i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); ++i)
		{
			meshTextures.push_back(textures[materialIndex][i]);
		}
	}

	return Mesh(vertices, indices, meshTextures);
}

void processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>* meshes, std::vector<std::vector<Texture>> textures)
{
	for (GLuint i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes->push_back(processMesh(mesh, scene, textures));
	}

	for (GLuint i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene, meshes, textures);
	}
}

Model loadModel(std::string path, std::string textureDirectory, Animation* animation = nullptr)
{
	// Assimp importer only imports one scene per instance of importer
	// importing another scene will clear the data of that importer
	// TODO: better comments?
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR: ASSIMP: " << importer.GetErrorString() << std::endl;
		return Model();
	}

	// process material
	// TODO: add some better comments
	std::vector<std::vector<Texture>> textures;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];
		textures.push_back(loadMaterialTexture(material, i, aiTextureType_DIFFUSE, textureDirectory, animation));
	}

	std::vector<Mesh> meshes;
	processNode(scene->mRootNode, scene, &meshes, textures);

	return Model(meshes);
}

Animation loadAnimation(std::string path, std::vector<std::string> modelNames, std::string textureDirectory)
{
	Animation animation;

	std::clock_t overallStart = clock();

	for (GLuint i = 0; i < modelNames.size(); ++i)
	{
		std::clock_t begin = clock();

		Model model = loadModel(path + modelNames[i], textureDirectory, &animation);
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