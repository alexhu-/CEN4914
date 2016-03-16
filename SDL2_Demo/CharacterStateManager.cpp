#include "CharacterStateManager.h"

CharacterStateManager::CharacterStateManager(CharacterData* characterData) :
	mState(CharacterState()),
	mData(characterData),
	mInputs(GameInput::INPUT_NONE),
	mJumpTimer(0)
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
			mState.setAction(Action::ACTION_KNOCKDOWN);
		}
	}

	if (mState.getStatus() != Status::STATUS_NONE || mState.getAction() != Action::ACTION_NONE)
	{
		canAttack = false;
	}

	// update state relating to attacks
	updateAttack(canAttack, stateVerticalDirection);

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