#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter(CharacterData* data, Model* model) :
	mData(data),
	mModel(model),
	mStateManager(CharacterStateManager(mData)),
	mPositionx(0.0f),
	mPositiony(0.0f),
	mPositionz(0.0f)
{

}

void PlayerCharacter::setup()
{
	mModel->setup();
}

void PlayerCharacter::cleanup()
{
	mModel->clearGLBuffers();
}

void PlayerCharacter::draw(GLint sampler, GLint modelUniform)
{
	mModel->draw(sampler, modelUniform, glm::mat4(1.0f));
}

std::vector<glm::mat4> PlayerCharacter::getBoneTransforms(unsigned int timeInMS)
{
	return mModel->getBoneTransforms(timeInMS, 0);
}

void PlayerCharacter::translateCharacter(float x, float y, float z)
{
	mPositionx += x;
	mPositiony += y;
	mPositionz += z;
	
	mModel->translateInWorld(x, y, z);
}

float PlayerCharacter::getX()
{
	return mPositionx;
}

float PlayerCharacter::getY()
{
	return mPositiony;
}

float PlayerCharacter::getZ()
{
	return mPositionz;
}

void PlayerCharacter::setInputs(unsigned int inputs)
{
	mStateManager.setGameInputs(inputs);
}

MoveSet PlayerCharacter::getMoveSet()
{
	return mStateManager.getMoveSet();
}

void PlayerCharacter::updateState()
{
	mStateManager.update();
}

void PlayerCharacter::updatePosition(float forwardSpeed, float backwardSpeed, float jumpUpwardSpeed, float jumpForwardSpeed)
{
	CharacterState state = mStateManager.getState();
	if (state.getHorizontalDirection() == HorizontalDirection::HDIRECTION_FORWARD && state.getVerticalDirection() == VerticalDirection::VDIRECTION_STAND  && state.getAction() == Action::ACTION_NONE)
	{
		this->translateCharacter(forwardSpeed, 0.0f, 0.0f);
	}
	if (state.getHorizontalDirection() == HorizontalDirection::HDIRECTION_BACKWARD && state.getVerticalDirection() == VerticalDirection::VDIRECTION_STAND && state.getAction() == Action::ACTION_NONE)
	{
		this->translateCharacter(-1.0f * backwardSpeed, 0.0f, 0.0f);
	}
	if (state.getHorizontalDirection() == HorizontalDirection::HDIRECTION_FORWARD && state.getVerticalDirection() == VerticalDirection::VDIRECTION_JUMP)
	{
		unsigned int jumpTime = mStateManager.getJumpDuration();
		unsigned int jumpStartup = mData->getTotalStartupFrames(MoveSet::JUMP);
		unsigned int jumpActive = mData->getTotalActiveFrames(MoveSet::JUMP);
		float halfActive = (float)jumpActive / 2.0f;
		if (jumpTime > jumpStartup && jumpTime <  jumpStartup + jumpActive)
		{
			float yinc = 2.0f - 2.0f * (float)(jumpTime - 3) / halfActive;
			this->translateCharacter(jumpForwardSpeed, jumpUpwardSpeed * yinc, 0.0f);
		}
	}
	if (state.getHorizontalDirection() == HorizontalDirection::HDIRECTION_BACKWARD && state.getVerticalDirection() == VerticalDirection::VDIRECTION_JUMP)
	{
		unsigned int jumpTime = mStateManager.getJumpDuration();
		unsigned int jumpStartup = mData->getTotalStartupFrames(MoveSet::JUMP);
		unsigned int jumpActive = mData->getTotalActiveFrames(MoveSet::JUMP);
		float halfActive = (float)jumpActive / 2.0f;
		if (jumpTime > jumpStartup && jumpTime <  jumpStartup + jumpActive)
		{
			float yinc = 2.0f - 2.0f * (float)(jumpTime - 3) / halfActive;
			this->translateCharacter(-1.0f * jumpForwardSpeed, jumpUpwardSpeed * yinc, 0.0f);
		}
	}
	if (state.getHorizontalDirection() == HorizontalDirection::HDIRECTION_NEUTRAL && state.getVerticalDirection() == VerticalDirection::VDIRECTION_JUMP)
	{
		unsigned int jumpTime = mStateManager.getJumpDuration();
		unsigned int jumpStartup = mData->getTotalStartupFrames(MoveSet::JUMP);
		unsigned int jumpActive = mData->getTotalActiveFrames(MoveSet::JUMP);
		float halfActive = (float)jumpActive / 2.0f;
		if (jumpTime > jumpStartup && jumpTime <  jumpStartup + jumpActive)
		{
			float yinc = 2.0f - 2.0f * (float)(jumpTime - jumpStartup) / halfActive;;
			this->translateCharacter(0.0f, jumpUpwardSpeed * yinc, 0.0f);
		}
	}
}

bool PlayerCharacter::shouldChangeAnimation()
{
	return mStateManager.shouldChangeAnimation();
}

void PlayerCharacter::setEventHit(unsigned int hitstun, unsigned int jumphitstun)
{
	mStateManager.setGameEvent(GameEvent::EVENT_HIT);
	if (mStateManager.getState().getVerticalDirection() == VerticalDirection::VDIRECTION_JUMP)
	{
		mStateManager.setHitTimer(jumphitstun);
	}
	else
	{
		mStateManager.setHitTimer(hitstun);
	}
}