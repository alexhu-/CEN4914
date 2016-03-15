#pragma once

enum VerticalDirection : int
{
	VDIRECTION_STAND = 0,
	VDIRECTION_CROUCH = 1,
	VDIRECTION_JUMP = 2
};

enum Action : int
{
	ACTION_NONE = 0,
	ACTION_BLOCK = 1,
	ACTION_HIT = 2,
	ACTION_KNOCKDOWN = 3,
	ACTION_PUNCH1 = 4,
	ACTION_PUNCH2 = 5,
	ACTION_KICK1 = 6,
	ACTION_KICK2 = 7
};

enum HorizontalDirection
{
	HDIRECTION_BACKWARD,
	HDIRECTION_FORWARD,
	HDIRECTION_NEUTRAL
};

enum Status
{
	STATUS_ACTIVE,
	STATUS_RECOVERY,
	STATUS_STARTUP,
	STATUS_NONE
};

class CharacterState
{
public:
	CharacterState();
	void setVerticalDirection(VerticalDirection vdirection);
	void setAction(Action action);
	void setHorizontalDirection(HorizontalDirection hdirection);
	void setStatus(Status status);
	void setStateTimer(unsigned int time);
	VerticalDirection getVerticalDirection();
	Action getAction();
	HorizontalDirection getHorizontalDirection();
	Status getStatus();
	unsigned int getStateTimer();

private:
	Action mCurrentAction;
	HorizontalDirection mCurrentHorizontalDirection;
	VerticalDirection mCurrentVerticalDirection;
	Status mCurrentStatus;
	unsigned int mStateTimer;
};