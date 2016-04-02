#pragma once

#include <glm\glm.hpp>

#include "CharacterData.h"
#include "CharacterStateManager.h"
#include "Model.h"

class PlayerCharacter
{
public:
	PlayerCharacter(CharacterData* data, Model* model);
	void setup();
	void cleanup();
	void draw(GLint sampler, GLint modelUniform);
	std::vector<glm::mat4> getBoneTransforms(unsigned int timeInMS);
	void translateCharacter(float x, float y, float z);
	float getX();
	float getY();
	float getZ();
	void setInputs(unsigned int inputs);
	MoveSet getMoveSet();
	void updateState();
	void updatePosition(float forwardSpeed, float backwardSpeed, float jumpUpwardSpeed, float jumpForwardSpeed, float jumpBackwardSpeed);
	bool shouldChangeAnimation();
	void setEventHit(unsigned int hitstun, unsigned int jumphitstun, unsigned int blockstun);
	void setEventKnockdown();
	void swapDirections();

	void setDisplayMoveSetOn();

private:
	CharacterData* mData;
	CharacterStateManager mStateManager;
	Model* mModel;

	float mPositionx;
	float mPositiony;
	float mPositionz;
};