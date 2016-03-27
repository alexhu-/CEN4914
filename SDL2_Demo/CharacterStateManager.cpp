#include "CharacterStateManager.h"

CharacterStateManager::CharacterStateManager(CharacterData* characterData) :
	mState(CharacterState()),
	mData(characterData),
	mInputs(GameInput::INPUT_NONE),
	mJumpTimer(0),
	mGameEvent(GameEvent::EVENT_NONE)
{
	mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_NEUTRAL);
	mState.setVerticalDirection(VerticalDirection::VDIRECTION_STAND);
	mState.setAction(Action::ACTION_NONE);
	mState.setStateTimer(0);
}

void CharacterStateManager::setGameInputs(unsigned int inputs)
{
	mInputs = inputs;
}

void CharacterStateManager::update()
{
	// check to see if any changes in state should be ignored
	bool canAttack = true;
	bool isDoingAction = false;
	bool isJumping = false;

	VerticalDirection stateVerticalDirection = mState.getVerticalDirection();
	Action stateAction = mState.getAction();

	// force state to stand after landing from jumping
	if (stateVerticalDirection == VerticalDirection::VDIRECTION_JUMP &&  (mState.getStateTimer() + mJumpTimer) >= mData->getTotalFrames(MoveSet::JUMP))
	{
		// force standing and reset timer
		mState.setVerticalDirection(VerticalDirection::VDIRECTION_STAND);
		mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_NEUTRAL);
		mState.setAction(Action::ACTION_NONE);
		mState.setStatus(Status::STATUS_NONE);
		mState.setStateTimer(0);
		mJumpTimer = 0;

		// check to see if character is in hit state when it lands
		// if so, change the action to being knockdown
		if (stateAction == Action::ACTION_HIT)
		{
			//mJumpAndHit = 2;
			//mState.setAction(Action::ACTION_KNOCKDOWN);
		}
	}

	if (mState.getStatus() != Status::STATUS_NONE || mState.getAction() != Action::ACTION_NONE)
	{
		canAttack = false;
	}

	// update state relating to actions
	updateAction(canAttack, stateVerticalDirection, stateAction, mState.getStateTimer());

	// update all booleans if character is attacking
	stateVerticalDirection = mState.getVerticalDirection();
	stateAction = mState.getAction();

	if (stateVerticalDirection == VerticalDirection::VDIRECTION_JUMP)
	{
		isJumping = true;
		updateJump(isJumping, mState.getStateTimer() + mJumpTimer);
	}

	if (stateAction != Action::ACTION_NONE)
	{
		isDoingAction = true;
	}

	// update any directions if necessary
	updateDirection(isDoingAction, isJumping);

	// check game events and change states appropriately
	updateGameEvent(stateVerticalDirection, mState.getStateTimer());

	// reset game event after updating it
	mGameEvent = GameEvent::EVENT_NONE;

	// increment the state duration everytime this function (update) is called
	mState.setStateTimer(mState.getStateTimer() + 1);
}

void CharacterStateManager::updateJump(bool isJumping, unsigned int stateTime)
{
	if (!isJumping)
	{
		return;
	}

	unsigned int jumpStartup = mData->getTotalStartupFrames(MoveSet::JUMP);

	if (stateTime < jumpStartup)
	{
		mState.setStatus(Status::STATUS_STARTUP);
	}
	else if (stateTime < jumpStartup + mData->getTotalActiveFrames(MoveSet::JUMP))
	{
		mState.setStatus(Status::STATUS_NONE);
	}
	else
	{
		mState.setStatus(Status::STATUS_RECOVERY);
	}
}

void CharacterStateManager::setAttackAction(Action action)
{
	// set action and reset timer
	// force status to be startup as we are just starting the move
	if (mState.getVerticalDirection() == VerticalDirection::VDIRECTION_JUMP)
	{
		mJumpTimer = mState.getStateTimer();
	}
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

void CharacterStateManager::updateAttack(bool canAttack, VerticalDirection verticalState)
{
	// first check if the character is already attacking
	// if attacking, change state to startup, active, or recovery

	unsigned int direction = 0;

	if (verticalState == VerticalDirection::VDIRECTION_CROUCH)
	{
		direction = 4;
	}

	if (verticalState == VerticalDirection::VDIRECTION_JUMP)
	{
		direction = 8;
	}

	switch (mState.getAction())
	{
	case Action::ACTION_PUNCH1 : 
		updateAttackStatus(MoveSet::STAND_PUNCH1 + direction);
		break;
	case Action::ACTION_PUNCH2 :
		updateAttackStatus(MoveSet::STAND_PUNCH2 + direction);
		break;
	case Action::ACTION_KICK1 :
		updateAttackStatus(MoveSet::STAND_KICK1 + direction);
		break;
	case Action::ACTION_KICK2 :
		updateAttackStatus(MoveSet::STAND_KICK2 + direction);
		break;
	default: 
		break;
	}

	if (!canAttack)
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

void CharacterStateManager::updateDirection(bool isDoingAction, bool isJumping)
{
	if (isDoingAction || isJumping)
	{
		return;
	}

	if (mInputs & GameInput::INPUT_RIGHT)
	{
		if (mState.getHorizontalDirection() != HorizontalDirection::HDIRECTION_FORWARD)
		{
			mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_FORWARD);
			mState.setStateTimer(0);
			mState.setStatus(Status::STATUS_NONE);
		}
	}
	else if (mInputs & GameInput::INPUT_LEFT)
	{
		if (mState.getHorizontalDirection() != HorizontalDirection::HDIRECTION_BACKWARD)
		{
			mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_BACKWARD);
			mState.setStateTimer(0);
			mState.setStatus(Status::STATUS_NONE);
		}
	}
	else if (mInputs == GameInput::INPUT_NONE || !( mInputs & GameInput::INPUT_RIGHT || mInputs & GameInput::INPUT_LEFT ))
	{
		if (mState.getHorizontalDirection() != HorizontalDirection::HDIRECTION_NEUTRAL)
		{
			mState.setHorizontalDirection(HorizontalDirection::HDIRECTION_NEUTRAL);
			mState.setStateTimer(0);
			mState.setStatus(Status::STATUS_NONE);
		}
	}

	if (mInputs & GameInput::INPUT_DOWN)
	{
		if (mState.getVerticalDirection() != VerticalDirection::VDIRECTION_CROUCH)
		{
			mState.setVerticalDirection(VerticalDirection::VDIRECTION_CROUCH);
			mState.setStatus(Status::STATUS_NONE);
			mState.setStateTimer(0);
		}
	}
	else if (mInputs & GameInput::INPUT_UP)
	{
		// obtain character data to set jump duration
		mState.setVerticalDirection(VerticalDirection::VDIRECTION_JUMP);
		mState.setStateTimer(0);
		mState.setStatus(Status::STATUS_STARTUP);
		mJumpTimer = 0;
	}
	else if (mInputs == GameInput::INPUT_NONE || !( mInputs & GameInput::INPUT_UP || mInputs & GameInput::INPUT_DOWN ))
	{
		if (mState.getVerticalDirection() != VerticalDirection::VDIRECTION_STAND)
		{
			mState.setVerticalDirection(VerticalDirection::VDIRECTION_STAND);
			mState.setStateTimer(0);
			mState.setStatus(Status::STATUS_NONE);
		}
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
		return mJumpTimer + mState.getStateTimer();
	}
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
		unsigned int actionHit = (unsigned int) MoveSet::STAND_HIT + (unsigned int) vdir + mJumpAndHit;
		move = (MoveSet) actionHit;
		mJumpAndHit = 0;
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

void CharacterStateManager::updateGameEvent(VerticalDirection verticalState, unsigned int stateTime)
{
	if (mGameEvent == GameEvent::EVENT_HIT)
	{
		mState.setAction(Action::ACTION_HIT);

		if (verticalState == VerticalDirection::VDIRECTION_JUMP)
		{
			mJumpTimer = stateTime;
		}

		mState.setStateTimer(0);
	}
}

void CharacterStateManager::setHitTimer(unsigned int hitstun)
{
	mHitTimer = hitstun;
}

unsigned int CharacterStateManager::getHitTimer()
{
	return mHitTimer;
}

void CharacterStateManager::updateAction(bool canAttack, VerticalDirection verticalState, Action action, unsigned int stateTime)
{
	if (action == Action::ACTION_HIT && stateTime >= mHitTimer)
	{
		mState.setAction(Action::ACTION_NONE);
		mState.setStatus(Status::STATUS_NONE);

		if (verticalState == VerticalDirection::VDIRECTION_JUMP)
		{
			mJumpTimer = stateTime;
		}

		mState.setStateTimer(0);
	}

	if (action == Action::ACTION_KNOCKDOWN && stateTime >= mData->getTotalFrames(MoveSet::STAND_KNOCKDOWN))
	{
		mState.setAction(Action::ACTION_NONE);
		mState.setStatus(Status::STATUS_NONE);
		mState.setStateTimer(0);
	}

	if (mState.getAction() != Action::ACTION_HIT)
	{
		updateAttack(canAttack, verticalState);
	}
}