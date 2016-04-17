#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter(CharacterData* data, Model* model) :
	mData(data),
	mModel(model),
	mStateManager(CharacterStateManager(mData)),
	mPositionx(0.0f),
	mPositiony(0.0f),
	mPositionz(0.0f),
	health(100),
	isHit(false)
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

void PlayerCharacter::updatePosition(float forwardSpeed, float backwardSpeed, float jumpUpwardSpeed, float jumpForwardSpeed, float jumpBackwardSpeed)
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
			this->translateCharacter(-1.0f * jumpBackwardSpeed, jumpUpwardSpeed * yinc, 0.0f);
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

void PlayerCharacter::setEventHit(unsigned int hitstun, unsigned int jumphitstun, unsigned int blockstun)
{
	mStateManager.setGameEvent(GameEvent::EVENT_HIT);
	if (mStateManager.getState().getVerticalDirection() == VerticalDirection::VDIRECTION_JUMP)
	{
		mStateManager.setHitTimer(jumphitstun, blockstun);
	}
	else
	{
		mStateManager.setHitTimer(hitstun, blockstun);
	}
}

void PlayerCharacter::setDisplayMoveSetOn()
{
	mStateManager.displayMoveSet = true;
}

void PlayerCharacter::setEventKnockdown()
{
	if (mStateManager.getState().getVerticalDirection() == VerticalDirection::VDIRECTION_JUMP)
	{
		return;
	}
	mStateManager.setGameEvent(GameEvent::EVENT_KNOCKDOWN);
}

void PlayerCharacter::swapDirections()
{
	mStateManager.swapDirections();
}

const char* PlayerCharacter::getStatusName()
{
	Status mStatus = mStateManager.getStatus();
	switch (mStatus)
	{
	case STATUS_ACTIVE: return "active";
	case STATUS_RECOVERY: return "recovery";
	case STATUS_STARTUP: return "startup";
	case STATUS_NONE: return "none";
	}
}
Status PlayerCharacter::getStatus()
{
	return mStateManager.getStatus();
}
Action PlayerCharacter::getAction()
{
	return mStateManager.getAction();
}
const char* PlayerCharacter::getActionName()
{
	Action mAction = mStateManager.getAction();
	switch (mAction)
	{
	case ACTION_NONE: return "none";
	case ACTION_BLOCK: return "block";
	case ACTION_HIT: return "hit";
	case ACTION_KNOCKDOWN: return "knockdown";
	case ACTION_PUNCH1: return "punch1";
	case ACTION_PUNCH2: return "punch2";
	case ACTION_KICK1: return "kick1";
	case ACTION_KICK2: return "kick2";
	}
}
VerticalDirection PlayerCharacter::getVerticalDirection() 
{
	return mStateManager.getVerticalDirection();
}

int PlayerCharacter::getHealth()
{
	
	if (health <= 0)
		return 0;
	else { return health; }
}

void PlayerCharacter::reduceHealth(int damage)
{
	health -= damage;
}

unsigned int PlayerCharacter::getStateTime()
{
	return mStateManager.getStateTime();
}

void PlayerCharacter::setIsHit(bool isHit)
{
	this->isHit = isHit;
}

bool PlayerCharacter::getIsHit()
{
	return isHit;
}
