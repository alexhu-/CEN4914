#include "CharacterState.h"

CharacterState::CharacterState() :
	mCurrentAction(Action::ACTION_NONE),
	mCurrentHorizontalDirection(HorizontalDirection::HDIRECTION_NEUTRAL),
	mCurrentVerticalDirection(VerticalDirection::VDIRECTION_STAND),
	mCurrentStatus(Status::STATUS_NONE),
	mStateTimer(0)
{

}

void CharacterState::setVerticalDirection(VerticalDirection vdirection)
{
	mCurrentVerticalDirection = vdirection;
}

void CharacterState::setAction(Action action)
{
	mCurrentAction = action;
}

void CharacterState::setHorizontalDirection(HorizontalDirection hdirection)
{
	mCurrentHorizontalDirection = hdirection;
}

void CharacterState::setStatus(Status status)
{
	mCurrentStatus = status;
}

void CharacterState::setStateTimer(unsigned int time)
{
	mStateTimer = time;
}

VerticalDirection CharacterState::getVerticalDirection()
{
	return mCurrentVerticalDirection;
}

Action CharacterState::getAction()
{
	return mCurrentAction;
}

HorizontalDirection CharacterState::getHorizontalDirection()
{
	return mCurrentHorizontalDirection;
}

unsigned int CharacterState::getStateTimer()
{
	return mStateTimer;
}

Status CharacterState::getStatus()
{
	return mCurrentStatus;
}