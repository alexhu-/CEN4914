#include "HitEffect.h"

HitEffect::HitEffect(Model* model) :
	mModel(model),
	mPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
	mTime(0),
	mEffectTime(10)
{
	mModel->setup();
	mModel->scale(0.1f, 0.1f, 0.4f);
	mModel->rotateX(90.0f);
}

void HitEffect::setPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;

	mModel->setWorldTranslateMatrix(x, y, z);
}

void HitEffect::setEffectTime(unsigned int time)
{
	mEffectTime = time;
}

void HitEffect::draw(GLint sampler, GLint modelUniform)
{
	++mTime;
	/*if (mTime >= mEffectTime)
	{
		mTime = 0;
		mModel->setWorldScaleMatrix(1.0f, 1.0f, 1.0f);
	}*/
	float scaleConst = 0.1 * mTime;
	mModel->setWorldScaleMatrix(scaleConst, scaleConst, 1.0f);
	mModel->draw(sampler, modelUniform, glm::mat4(1.0f));
}

bool HitEffect::isEffectOver()
{
	return mTime >= mEffectTime;
}

void HitEffect::resetTimer()
{
	mTime = 0;
}

void HitEffect::setTime(unsigned int time)
{
	mTime = time;
}