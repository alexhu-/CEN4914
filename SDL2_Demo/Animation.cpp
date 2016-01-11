#include "Animation.h"

Animation::Animation() :
	mTextures(),
	mModels(),
	mIsTextureLoadedIndexCache(0),
	mCurrentFrame(0),
	mPlayAnimation(false)
{

}

void Animation::addModel(Model model)
{
	mModels.push_back(model);
}

void Animation::addTexture(Texture texture)
{
	mTextures.push_back(texture);
}

bool Animation::isTextureLoaded(std::string textureFileName)
{
	for (unsigned int i = 0; i < mTextures.size(); ++i)
	{
		if (mTextures[i].fileName == textureFileName)
		{
			mIsTextureLoadedIndexCache = i;
			return true;
		}
	}

	return false;
}

Texture* Animation::getTexture(std::string textureFileName)
{
	if (mTextures[mIsTextureLoadedIndexCache].fileName == textureFileName)
	{
		return &mTextures[mIsTextureLoadedIndexCache];
	}

	for (unsigned int i = 0; i < mTextures.size(); ++i)
	{
		if (mTextures[i].fileName == textureFileName)
		{
			return &mTextures[i];
		}
	}

	return nullptr;
}

void Animation::toggleAnimation()
{
	mPlayAnimation = !mPlayAnimation;
}

void Animation::pauseAnimation()
{
	mPlayAnimation = false;
}

void Animation::resetAnimation()
{
	mCurrentFrame = 0;
}

void Animation::nextItem()
{
	if (!mPlayAnimation)
	{
		return;
	}

	++mCurrentFrame;
	mCurrentFrame = mCurrentFrame % mModels.size();
}

bool Animation::isAnimationPlaying()
{
	return mPlayAnimation;
}

Model* Animation::currentItem()
{
	if (!mModels.empty())
	{
		return &mModels[mCurrentFrame];
	}

	return nullptr;
}

void Animation::clearGLBuffers()
{
	if (!mModels.empty())
	{
		for (int i = 0; i < mModels.size(); ++i)
		{
			mModels[i].clearGLBuffers();
		}
	}
}