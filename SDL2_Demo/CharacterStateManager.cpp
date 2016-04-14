#include "CharacterStateManager.h"

CharacterStateManager::CharacterStateManager(CharacterData* characterData) :
	mState(CharacterState()),
	mData(characterData),
	mInputs(GameInput::INPUT_NONE),
	mJumpTimer(0),
	mHitTimer(0),
	mBlockTimer(0),
	mGameEvent(GameEvent::EVENT_NONE),
	mBackwardDirection(GameInput::INPUT_LEFT),
	mForwardDirection(GameInput::INPUT_RIGHT)
{
	mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_NEUTRAL);
	mState.setVerticalDirection(VerticalDirection::VDIRECTION_STAND);
	mState.setAction(Action::ACTION_NONE);
	mState.setStateTimer(0);

	displayMoveSet = false;

	std::string temp[] = {
		"STAND_PUNCH1",
		"STAND_PUNCH2",
		"STAND_KICK1",
		"STAND_KICK2",
		"CROUCH_PUNCH1",
		"CROUCH_PUNCH2",
		"CROUCH_KICK1",
		"CROUCH_KICK2",
		"JUMP_PUNCH1",
		"JUMP_PUNCH2",
		"JUMP_KICK1",
		"JUMP_KICK2",
		"IDLE",
		"WALK_FORWARD",
		"WALK_BACKWARD",
		"JUMP",
		"CROUCH",
		"STAND_BLOCK",
		"CROUCH_BLOCK",
		"STAND_HIT",
		"CROUCH_HIT",
		"JUMP_HIT",
		"STAND_KNOCKDOWN",
		"CROUCH_KNOCKDOWN",
		"JUMP_KNOCKDOWN"
	};

	std::string temp2[] = {
		"ACTION_NONE",
		"ACTION_BLOCK",
		"ACTION_HIT",
		"ACTION_KNOCKDOWN",
		"ACTION_PUNCH1",
		"ACTION_PUNCH2",
		"ACTION_KICK1",
		"ACTION_KICK2"
	};

	for (unsigned int i = 0; i != 25; ++i)
	{
		moveSetString[i] = temp[i];
	}

	for (unsigned int i = 0; i != 8; ++i)
	{
		actionString[i] = temp2[i];
	}
}

void CharacterStateManager::setGameInputs(unsigned int inputs)
{
	mInputs = inputs;
}

void CharacterStateManager::update()
{
	updateDirection();
	updateAction(mState.getHorizontalDirection(), mState.getVerticalDirection(), mState.getAction(), mState.getStateTimer());

	// reset game event after updating it
	mGameEvent = GameEvent::EVENT_NONE;

	// increment the state duration everytime this function (update) is called
	mState.setStateTimer(mState.getStateTimer() + 1);
}

void CharacterStateManager::setAttackAction(Action action)
{
	// set action and reset timer
	// force status to be startup as we are just starting the move
	mState.setAction(action);
	mState.setStateTimer(0);
	mState.setStatus(Status::STATUS_STARTUP);
}

void CharacterStateManager::updateAttackStatus(unsigned int attackIndex)
{
	unsigned int stateTime = mState.getStateTimer();
	unsigned int startup = mData->getTotalStartupFrames(attackIndex);
	if (stateTime < startup)
	{
		mState.setStatus(Status::STATUS_STARTUP);
	}
	else if (stateTime < startup + mData->getTotalActiveFrames(attackIndex))
	{
		mState.setStatus(Status::STATUS_ACTIVE);
	}
	else if (stateTime < mData->getTotalFrames(attackIndex))
	{
		mState.setStatus(Status::STATUS_RECOVERY);
	}
	else
	{
		mState.setStatus(Status::STATUS_NONE);
		mState.setAction(Action::ACTION_NONE);
		mState.setStateTimer(0);
	}
}

CharacterState CharacterStateManager::getState()
{
	return mState;
}

unsigned int CharacterStateManager::getJumpDuration()
{
	if (mState.getVerticalDirection() == VerticalDirection::VDIRECTION_JUMP)
	{
		//return mJumpTimer + mState.getStateTimer();
		return mJumpTimer;
	}

	return 0;
}

unsigned int CharacterStateManager::getStateTime()
{
	return mState.getStateTimer();
}

MoveSet CharacterStateManager::getMoveSet()
{
	Action action = mState.getAction();
	HorizontalDirection hdir = mState.getHorizontalDirection();
	VerticalDirection vdir = mState.getVerticalDirection();
	MoveSet move = MoveSet::IDLE;

	unsigned int controllerDirection = mState.getVerticalDirection() * 4;
	
	if (action == Action::ACTION_KNOCKDOWN)
	{
		unsigned int actionKD = (unsigned int)MoveSet::STAND_KNOCKDOWN + (unsigned int)vdir;
		move = (MoveSet)actionKD;
	}
	if (action == Action::ACTION_HIT)
	{
		unsigned int actionHit = (unsigned int) MoveSet::STAND_HIT + (unsigned int) vdir;
		move = (MoveSet) actionHit;
	}
	if (action == Action::ACTION_BLOCK)
	{
		unsigned int actionBlock = (unsigned int)MoveSet::STAND_BLOCK + (unsigned int) vdir;
		move = (MoveSet) actionBlock;
	}
	if (action == Action::ACTION_PUNCH1 ||
		action == Action::ACTION_PUNCH2 ||
		action == Action::ACTION_KICK1 ||
		action == Action::ACTION_KICK2)
	{
		unsigned int attack = action - 4 + controllerDirection;
		move = (MoveSet) attack;
	}
	if (hdir == HorizontalDirection::HDIRECTION_FORWARD && vdir == VerticalDirection::VDIRECTION_STAND && action == Action::ACTION_NONE)
	{
		move = MoveSet::WALK_FORWARD;
	}
	if (hdir == HorizontalDirection::HDIRECTION_BACKWARD && vdir == VerticalDirection::VDIRECTION_STAND && action == Action::ACTION_NONE)
	{
		move = MoveSet::WALK_BACKWARD;
	}
	if (vdir == VerticalDirection::VDIRECTION_JUMP && action == Action::ACTION_NONE)
	{
		move = MoveSet::JUMP;
	}
	if (vdir == VerticalDirection::VDIRECTION_CROUCH && action == Action::ACTION_NONE)
	{
		move = MoveSet::CROUCH;
	}
	if (vdir == VerticalDirection::VDIRECTION_STAND && hdir == HorizontalDirection::HDIRECTION_NEUTRAL && action == Action::ACTION_NONE)
	{
		move = MoveSet::IDLE;
	}

	if (displayMoveSet)
	{
		std::cout << actionString[action] << std::endl;
		std::cout << moveSetString[move] << std::endl;
	}
	return move;
}

bool CharacterStateManager::shouldChangeAnimation()
{
	return (mState.getStateTimer() == 1);
}

void CharacterStateManager::setGameEvent(GameEvent gameEvent)
{
	mGameEvent = gameEvent;
}

GameEvent CharacterStateManager::getGameEvent()
{
	return mGameEvent;
}

void CharacterStateManager::setHitTimer(unsigned int hitstun, unsigned int blockstun)
{
	mHitTimer = hitstun;
	mBlockTimer = blockstun;
}

unsigned int CharacterStateManager::getHitTimer()
{
	return mHitTimer;
}

void CharacterStateManager::updateJump(Action action, VerticalDirection verticalDirection)
{
	if (verticalDirection == VerticalDirection::VDIRECTION_JUMP)
	{
		unsigned int jumpStartup = mData->getTotalStartupFrames(MoveSet::JUMP);
		unsigned int jumpActive = mData->getTotalActiveFrames(MoveSet::JUMP);

		if (mJumpTimer < jumpStartup)
		{
			mState.setStatus(Status::STATUS_STARTUP);
		}
		else if (mJumpTimer < jumpStartup + jumpActive)
		{
			mState.setStatus(Status::STATUS_ACTIVE);
		}
		else if (mJumpTimer < jumpStartup + jumpActive + mData->getTotalRecoveryFrames(MoveSet::JUMP))
		{
			mState.setStatus(Status::STATUS_RECOVERY);
		}
		else
		{
			// force standing after landing from a jump
			mState.setStatus(Status::STATUS_NONE);
			mState.setVerticalDirection(VerticalDirection::VDIRECTION_STAND);
			mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_NEUTRAL);
			mState.setAction(Action::ACTION_NONE);
			mState.setStateTimer(0);
			mJumpTimer = 0;
		}

		if (action == Action::ACTION_HIT)
		{
			mState.setStatus(Status::STATUS_NONE);
		}

		// when jumping, increment jump timer
		++mJumpTimer;
	}
}

void CharacterStateManager::updateDirection()
{
	updateHorizontalDirection(mState.getAction(), mState.getHorizontalDirection(), mState.getVerticalDirection());
	updateVerticalDirection(mState.getAction(), mState.getVerticalDirection());
	updateJump(mState.getAction(), mState.getVerticalDirection());
}

void CharacterStateManager::updateVerticalDirection(Action action, VerticalDirection verticalDirection)
{
	// if the character is doing an action or jumping, we don't need to update the vertical direction state
	if ((action != Action::ACTION_NONE &&
		action != Action::ACTION_KNOCKDOWN &&
		action != Action::ACTION_HIT &&
		action != Action::ACTION_BLOCK) ||
		verticalDirection == VerticalDirection::VDIRECTION_JUMP)
	{
		return;
	}

	// if both up and down are pressed at the same time, crouching will have the most priority
	if ((mInputs & GameInput::INPUT_DOWN) &&
		(verticalDirection != VerticalDirection::VDIRECTION_CROUCH))
	{
		// if the player presses down and the character is not crouching, change state to crouch
		mState.setVerticalDirection(VerticalDirection::VDIRECTION_CROUCH);
		if (action != Action::ACTION_KNOCKDOWN && action != Action::ACTION_HIT && action != Action::ACTION_BLOCK)
		{
			mState.setStatus(Status::STATUS_NONE);
			mState.setStateTimer(0);
		}
	}
	else if ((mInputs & GameInput::INPUT_UP) &&
		(verticalDirection != VerticalDirection::VDIRECTION_JUMP) &&
		(action != Action::ACTION_KNOCKDOWN) && 
		(action != Action::ACTION_HIT) && 
		(action != Action::ACTION_BLOCK))
	{
		// if the player presses up and the character is not jumping, change state to jump
		// set player status to startup and start jump timer
		mState.setVerticalDirection(VerticalDirection::VDIRECTION_JUMP);
		mState.setStatus(Status::STATUS_STARTUP);
		mState.setStateTimer(0);
		mJumpTimer = 0;
	}
	else if (!(mInputs & GameInput::INPUT_DOWN || mInputs & GameInput::INPUT_UP) &&
		(verticalDirection != VerticalDirection::VDIRECTION_STAND))
	{
		// only stand when the player is not trying to jump or crouch
		mState.setVerticalDirection(VerticalDirection::VDIRECTION_STAND);
		if (action != Action::ACTION_KNOCKDOWN && action != Action::ACTION_HIT && action != Action::ACTION_BLOCK)
		{
			mState.setStatus(Status::STATUS_NONE);
			mState.setStateTimer(0);
		}
	}
}

void CharacterStateManager::updateHorizontalDirection(Action action, HorizontalDirection horizontalDirection, VerticalDirection verticalDirection)
{
	// if the character is doing an action or jumping, we don't need to update the horizontal direction state either
	if ((action != Action::ACTION_NONE &&
		action != Action::ACTION_KNOCKDOWN &&
		action != Action::ACTION_HIT &&
		action != Action::ACTION_BLOCK) ||
		verticalDirection == VerticalDirection::VDIRECTION_JUMP)
	{
		return;
	}

	if ((mInputs & mForwardDirection) &&
		(horizontalDirection != HorizontalDirection::HDIRECTION_FORWARD))
	{
		// walk in the forward direction if the player presses the forward button
		mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_FORWARD);
		if (action != Action::ACTION_KNOCKDOWN && action != Action::ACTION_HIT && action != Action::ACTION_BLOCK)
		{
			mState.setStatus(Status::STATUS_NONE);
			mState.setStateTimer(0);
		}
	}
	else if ((mInputs & mBackwardDirection) &&
		(horizontalDirection != HorizontalDirection::HDIRECTION_BACKWARD))
	{
		// walk in the backward direction if the player presses the backward button
		mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_BACKWARD);
		if (action != Action::ACTION_KNOCKDOWN && action != Action::ACTION_HIT && action != Action::ACTION_BLOCK)
		{
			mState.setStatus(Status::STATUS_NONE);
			mState.setStateTimer(0);
		}

	}
	else if (!(mInputs & mBackwardDirection|| mInputs & mForwardDirection) &&
		(horizontalDirection != HorizontalDirection::HDIRECTION_NEUTRAL))
	{
		// change to neutral position if the player does not press a direction
		mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_NEUTRAL);
		if (action != Action::ACTION_KNOCKDOWN && action != Action::ACTION_HIT && action != Action::ACTION_BLOCK)
		{
			mState.setStatus(Status::STATUS_NONE);
			mState.setStateTimer(0);
		}
	}
}

void CharacterStateManager::updateAction(HorizontalDirection horizontalDirection, VerticalDirection verticalDirection, Action action, unsigned int stateTime)
{
	if ((mGameEvent == GameEvent::EVENT_HIT || mGameEvent == GameEvent::EVENT_HIT_HIGH || mGameEvent == GameEvent::EVENT_HIT_LOW) &&
		action != Action::ACTION_KNOCKDOWN)
	{
		mState.setStatus(Status::STATUS_NONE);

		if (verticalDirection == VerticalDirection::VDIRECTION_JUMP && action != Action::ACTION_HIT)
		{
			mState.setAction(Action::ACTION_HIT);
			mState.setStateTimer(0);
		}
		else if (verticalDirection != VerticalDirection::VDIRECTION_JUMP)
		{
			// check to see if player is holding back and not doing anything else like getting hit or attacking
			bool holdingBackOnly = (horizontalDirection == HorizontalDirection::HDIRECTION_BACKWARD) && (action == Action::ACTION_NONE);

			if (action == Action::ACTION_BLOCK)
			{
				mState.setAction(Action::ACTION_BLOCK);
				mState.setStateTimer(0);
			}
			else if (holdingBackOnly &&
				verticalDirection == VerticalDirection::VDIRECTION_STAND &&
				(mGameEvent == GameEvent::EVENT_HIT || mGameEvent == GameEvent::EVENT_HIT_HIGH))
			{
				mState.setAction(Action::ACTION_BLOCK);
				mState.setStateTimer(0);
			}
			else if (holdingBackOnly &&
				verticalDirection == VerticalDirection::VDIRECTION_CROUCH &&
				(mGameEvent == GameEvent::EVENT_HIT || mGameEvent == GameEvent::EVENT_HIT_LOW))
			{
				mState.setAction(Action::ACTION_BLOCK);
				mState.setStateTimer(0);
			}
			else
			{
				mState.setAction(Action::ACTION_HIT);
				mState.setStateTimer(0);
			}
		}
	}

	if (mGameEvent == GameEvent::EVENT_KNOCKDOWN && action != Action::ACTION_KNOCKDOWN)
	{
		mState.setAction(Action::ACTION_KNOCKDOWN);
		mState.setStatus(Status::STATUS_NONE);
		mState.setStateTimer(0);
	}

	action = mState.getAction();
	stateTime = mState.getStateTimer();

	if (action == Action::ACTION_HIT)
	{
		if (verticalDirection != VerticalDirection::VDIRECTION_JUMP && stateTime >= mHitTimer)
		{
			mState.setAction(Action::ACTION_NONE);
			mState.setStateTimer(0);
		}
	}
	else if (action == Action::ACTION_KNOCKDOWN)
	{
		if (stateTime >= mData->getTotalFrames(MoveSet::STAND_KNOCKDOWN))
		{
			mState.setAction(Action::ACTION_NONE);
			mState.setStateTimer(0);
		}
	}
	else if (action == Action::ACTION_BLOCK)
	{
		if (stateTime >= mBlockTimer)
		{
			mState.setAction(Action::ACTION_NONE);
			mState.setStateTimer(0);
		}
	}
	else
	{
		updateAttack(verticalDirection, action);
	}
}

void CharacterStateManager::updateAttack(VerticalDirection verticalDirection, Action action)
{
	// first check if the character is already attacking
	// if attacking, change state to startup, active, or recovery
	unsigned int direction = 0;

	if (verticalDirection == VerticalDirection::VDIRECTION_CROUCH)
	{
		direction = 4;
	}

	if (verticalDirection == VerticalDirection::VDIRECTION_JUMP)
	{
		direction = 8;
	}

	switch (action)
	{
	case Action::ACTION_PUNCH1:
		updateAttackStatus(MoveSet::STAND_PUNCH1 + direction);
		break;
	case Action::ACTION_PUNCH2:
		updateAttackStatus(MoveSet::STAND_PUNCH2 + direction);
		break;
	case Action::ACTION_KICK1:
		updateAttackStatus(MoveSet::STAND_KICK1 + direction);
		break;
	case Action::ACTION_KICK2:
		updateAttackStatus(MoveSet::STAND_KICK2 + direction);
		break;
	default:
		break;
	}

	if (action != Action::ACTION_NONE)
	{
		return;
	}

	if (mInputs & GameInput::INPUT_PUNCH1)
	{
		setAttackAction(Action::ACTION_PUNCH1);
	}
	else if (mInputs & GameInput::INPUT_KICK1)
	{
		setAttackAction(Action::ACTION_KICK1);
	}
	else if (mInputs & GameInput::INPUT_PUNCH2)
	{
		setAttackAction(Action::ACTION_PUNCH2);
	}
	else if (mInputs & GameInput::INPUT_KICK2)
	{
		setAttackAction(Action::ACTION_KICK2);
	}
}

void CharacterStateManager::swapDirections()
{
	GameInput temp = mForwardDirection;
	mForwardDirection = mBackwardDirection;
	mBackwardDirection = temp;
	HorizontalDirection horizontalDir = mState.getHorizontalDirection();
	if (horizontalDir == HorizontalDirection::HDIRECTION_FORWARD)
	{
		mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_BACKWARD);
	}
	else if (horizontalDir == HorizontalDirection::HDIRECTION_BACKWARD)
	{
		mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_FORWARD);
	}
}

Status CharacterStateManager::getStatus()
{
	return mState.getStatus();
}

Action CharacterStateManager::getAction()
{
	return mState.getAction();
}

VerticalDirection CharacterStateManager::getVerticalDirection()
{
	return mState.getVerticalDirection();
}