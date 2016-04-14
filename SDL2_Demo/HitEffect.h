#pragma once

#include <glm/glm.hpp>

#include "Model.h"

class HitEffect
{
public:
	HitEffect(Model* model);
	void setPosition(float x, float y, float z);
	void setEffectTime(unsigned int time);
	void draw(GLint sampler, GLint modelUniform);
	bool isEffectOver();
	void resetTimer();
	void setTime(unsigned int time);

private:
	Model* mModel;
	glm::vec3 mPosition;
	unsigned int mTime;
	unsigned int mEffectTime;
};