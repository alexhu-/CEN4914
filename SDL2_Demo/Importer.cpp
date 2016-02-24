#include "Importer.h"

glm::mat4 Importer::glmMatrix(const aiMatrix4x4& matrix)
{
	glm::mat4 out(1.0f);
	out[0][0] = matrix.a1;
	out[0][1] = matrix.b1;
	out[0][2] = matrix.c1;
	out[0][3] = matrix.d1;
	out[1][0] = matrix.a2;
	out[1][1] = matrix.b2;
	out[1][2] = matrix.c2;
	out[1][3] = matrix.d2;
	out[2][0] = matrix.a3;
	out[2][1] = matrix.b3;
	out[2][2] = matrix.c3;
	out[2][3] = matrix.d3;
	out[3][0] = matrix.a4;
	out[3][1] = matrix.b4;
	out[3][2] = matrix.c4;
	out[3][3] = matrix.d4;

	return out;
}

// converts aiNode to Node Data - a data structure defined in Model.h
// creates new NodeData when copying the tree
void Importer::copyNodes(NodeData* out, const aiNode* node)
{
	unsigned int childrenCount = node->mNumChildren;

	out->name = node->mName.C_Str();
	out->transformation = glmMatrix(node->mTransformation);

	for (unsigned int i = 0; i < childrenCount; ++i)
	{
		out->addChild();
		copyNodes(out->children[i], node->mChildren[i]);
	}
}

// convert assimp's quaternion to glm quaternion
glm::quat Importer::glmQuaternion(const aiQuaternion& quat)
{
	float x = quat.x;
	float y = quat.y;
	float z = quat.z;
	float w = quat.w;
	return glm::quat(w, x, y, z);
}

// convert assimp's vector3 to glm vec3
glm::vec3 Importer::glmVec3(const aiVector3D& vector)
{
	float x = vector.x;
	float y = vector.y;
	float z = vector.z;
	return glm::vec3(x, y, z);
}

void Importer::loadBones(const aiMesh* mesh, std::vector<BoneData>* bones, std::map<std::string, GLuint>* boneMap, std::vector<VertexData>* vertices)
{
	for (unsigned int i = 0; i < mesh->mNumBones; ++i)
	{
		GLuint boneIndex = boneMap->size();
		std::string boneName(mesh->mBones[i]->mName.data);

		if (boneMap->find(boneName) == boneMap->end())
		{
			BoneData boneData;
			boneData.id = boneIndex;
			boneData.offset = glmMatrix(mesh->mBones[i]->mOffsetMatrix);
			bones->push_back(boneData);
			boneMap->insert(std::pair<std::string, GLuint>(boneName, boneData.id));
		}
		else
		{
			boneIndex = boneMap->at(boneName);
		}

		for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
		{
			unsigned int vertexIndex = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			vertices->at(vertexIndex).addWeight(boneIndex, weight);
		}
	}
}

GLuint Importer::loadTexture(std::string path, std::string directory)
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
		fileName = directory + "/" + cleanPath;
		image = SOIL_load_image(fileName.c_str(), &width, &height, &channel, SOIL_LOAD_RGB);
	}

	glBindTexture(GL_TEXTURE_2D, textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
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

std::vector<Texture> Importer::loadMaterialTexture(aiMaterial* material, GLuint materialIndex, aiTextureType type, std::string directory)
{
	std::vector<Texture> textures;
	for (GLuint i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString texturePath;
		material->GetTexture(type, i, &texturePath);
		std::string textureFileName = texturePath.C_Str();
		Texture texture;
		texture.id = loadTexture(textureFileName, directory);
		texture.materialIndex = materialIndex;
		texture.fileName = textureFileName;
		
		textures.push_back(texture);
	}

	return textures;
}

Mesh Importer::processMesh(aiMesh* mesh, const aiScene* scene, const std::vector<std::vector<Texture>>& textures, std::vector<BoneData>* bones, std::map<std::string, GLuint>* boneMap)
{
	// process mesh vertex data: position, normal, and texture coordinate
	std::vector<VertexData> vertices = processVertices(mesh);

	// process mesh indicies
	// go through each of the mesh's faces and retrieve the corresponding vertex indicies
	std::vector<GLuint> indices;
	for (GLuint i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];

		for (GLuint j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// process mesh textures
	// first check if there is a material associated with this mesh
	// if there is, retrieve this information from the loaded textures
	std::vector<Texture> meshTextures;
	if (mesh->mMaterialIndex >= 0)
	{
		unsigned int materialIndex = mesh->mMaterialIndex;
		aiMaterial* material = scene->mMaterials[materialIndex];
		GLuint textureCount = material->GetTextureCount(aiTextureType_DIFFUSE);
		meshTextures.resize(textureCount);
		for (GLuint i = 0; i < textureCount; ++i)
		{
			meshTextures[i] = textures[materialIndex][i];
		}
	}

	// process bones
	loadBones(mesh, bones, boneMap, &vertices);

	// return the mesh
	return Mesh(vertices, indices, meshTextures);
}

std::vector<VertexData> Importer::processVertices(aiMesh* mesh)
{
	std::vector<VertexData> vertices;
	vertices.resize(mesh->mNumVertices);

	for (GLuint i = 0; i < mesh->mNumVertices; ++i)
	{
		VertexData vertexData;
		glm::vec3 vector(0.0f, 0.0f, 0.0f);

		// the position of the mesh
		if (mesh->HasPositions())
		{
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
		}
		vertexData.position = vector;

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
		if (mesh->mTextureCoords[0])
		{
			// vertex can have up to 8 different texture coordinates... we will just use first set
			textureCoord.x = mesh->mTextureCoords[0][i].x;
			textureCoord.y = mesh->mTextureCoords[0][i].y;
		}
		vertexData.textureCoordinate = textureCoord;

		vertices[i] = vertexData;
	}

	return vertices;
}

void Importer::processNode(aiNode* node, const aiScene* scene, std::vector<Mesh>* meshes, std::vector<std::vector<Texture>> textures, std::vector<BoneData>* bones, std::map<std::string, GLuint>* boneMap)
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

Model Importer::loadModel(std::string path, std::string textureDirectory)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	// Check if scene exists
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		// Might be better to throw an exception
		std::cout << "ERROR: ASSIMP: " << importer.GetErrorString() << std::endl;
		return Model();
	}

	glm::mat4 globalInverseTransform = glmMatrix(scene->mRootNode->mTransformation);
	globalInverseTransform = glm::inverse(globalInverseTransform);

	// Process materials
	std::vector<std::vector<Texture>> textures = processTextures(scene, textureDirectory);

	// Process animations
	std::vector<AnimationEntry> animationEntries;
	if (scene->HasAnimations())
	{
		animationEntries = processAnimations(scene);
	}

	// Process nodes, mesh, and bones
	std::map<std::string, GLuint> boneMap;
	std::vector<BoneData> bones;
	std::vector<Mesh> meshes;
	processNode(scene->mRootNode, scene, &meshes, textures, &bones, &boneMap);

	Model model(animationEntries, boneMap, bones, meshes, globalInverseTransform);

	// Process keyframes
	copyNodes(model.getRoot(), scene->mRootNode);
	if (scene->HasAnimations())
	{
		processKeyframes(model.getRoot(), scene);
	}

	return model;
}

std::vector<AnimationEntry> Importer::processAnimations(const aiScene* scene)
{
	unsigned int animationCount = scene->mNumAnimations;
	std::vector<AnimationEntry> entries;
	entries.resize(animationCount);
	for (unsigned int i = 0; i < animationCount; ++i)
	{
		AnimationEntry entry;
		entry.duration = (float) scene->mAnimations[i]->mDuration;
		entry.ticksPerSecond =  (float) scene->mAnimations[i]->mTicksPerSecond;
		entries[i] = entry;
	}

	return entries;
}

std::vector<std::vector<Texture>> Importer::processTextures(const aiScene* scene, std::string textureDirectory)
{
	unsigned int materialsCount = scene->mNumMaterials;
	std::vector<std::vector<Texture>> textures;
	textures.resize(materialsCount);
	for (unsigned int i = 0; i < materialsCount; ++i)
	{
		aiMaterial* material = scene->mMaterials[i];
		textures[i] = loadMaterialTexture(material, i, aiTextureType_DIFFUSE, textureDirectory);
	}

	return textures;
}

void Importer::processKeyframes(NodeData* out, const aiScene* scene)
{
	unsigned int animationCount = scene->mNumAnimations;

	for (unsigned int i = 0; i < animationCount; ++i)
	{
		const aiAnimation* animation = scene->mAnimations[i];
		copyAnimationData(*out, animation);
	}

	unsigned int childrenCount = out->children.size();
	for (unsigned int i = 0; i < childrenCount; ++i)
	{
		processKeyframes(out->children[i], scene);
	}
}

void Importer::copyAnimationData(NodeData& out, const aiAnimation* animation)
{
	for (unsigned int i = 0; i < animation->mNumChannels; ++i)
	{
		const aiNodeAnim* nodeAnim = animation->mChannels[i];

		if (out.name == std::string(nodeAnim->mNodeName.data))
		{
			unsigned int positionCount = nodeAnim->mNumPositionKeys;
			unsigned int rotationCount = nodeAnim->mNumRotationKeys;
			unsigned int scalingCount = nodeAnim->mNumScalingKeys;
			AnimationData animationData(positionCount, rotationCount, scalingCount);

			for (unsigned int j = 0; j < positionCount; ++j)
			{
				animationData.positionTimes[j] = (float) nodeAnim->mPositionKeys[j].mTime;
				animationData.positionValues[j] = glmVec3(nodeAnim->mPositionKeys[j].mValue);
			}

			for (unsigned int j = 0; j < rotationCount; ++j)
			{
				animationData.quaternionTimes[j] = (float)nodeAnim->mRotationKeys[j].mTime;
				animationData.quaternionValues[j] = glmQuaternion(nodeAnim->mRotationKeys[j].mValue);
			}

			for (unsigned int j = 0; j < scalingCount; ++j)
			{
				animationData.scalingTimes[j] = (float)nodeAnim->mScalingKeys[j].mTime;
				animationData.scalingValues[j] = glmVec3(nodeAnim->mScalingKeys[j].mValue);
			}

			out.animations.push_back(animationData);
			break;
		}
	}
}