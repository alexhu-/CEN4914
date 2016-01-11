#pragma once

#include "Mesh.h"
#include "Model.h"

class Animation
{
public:
	Animation();
	void addModel(Model model);
	void addTexture(Texture texture);
	bool isTextureLoaded(std::string textureFileName);
	Texture* getTexture(std::string textureFileName);
	void toggleAnimation();
	void pauseAnimation();
	void resetAnimation();
	void nextItem();
	bool isAnimationPlaying();
	Model* currentItem();
	void clearGLBuffers();
private:
	std::vector<Texture> mTextures;
	std::vector<Model> mModels;
	unsigned int mIsTextureLoadedIndexCache;
	unsigned int mCurrentFrame;
	bool mPlayAnimation;
};