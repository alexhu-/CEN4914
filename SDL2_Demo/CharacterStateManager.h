#pragma once

#include <iostream>
#include <string>


#include "CharacterData.h"
#include "CharacterState.h"

enum GameInput : unsigned int
{
	INPUT_NONE = 0x0000,
	INPUT_UP = 0x0001,
	INPUT_DOWN = 0x0002,
	INPUT_LEFT = 0x0004,
	INPUT_RIGHT = 0x0008,
	INPUT_PUNCH1 = 0x0010,
	INPUT_PUNCH2 = 0x0020,
	INPUT_KICK1 = 0x0040,
	INPUT_KICK2 = 0x0080
};

enum GameEvent
{
	EVENT_NONE,
	EVENT_HIT_HIGH,
	EVENT_HIT_LOW,
	EVENT_HIT,
	EVENT_KNOCKDOWN
};

class CharacterStateManager
{
public:
	CharacterStateManager(CharacterData* characterData);
	void setGameInputs(unsigned int inputs);
	void update();
	CharacterState getState();
	unsigned int getJumpDuration();
	MoveSet getMoveSet();
	unsigned int getStateTime();
	bool shouldChangeAnimation();
	void setGameEvent(GameEvent gameEvent);
	GameEvent getGameEvent();
	void setHitTimer(unsigned int hitstun, unsigned int blockstun);
	unsigned int getHitTimer();
	void swapDirections();

	bool displayMoveSet;

private:
	//void updateJump(bool isJumping, unsigned int stateTime);
	//void updateAttack(bool canAttack, VerticalDirection verticalState);
	void setAttackAction(Action action);
	void updateAttackStatus(unsigned int attackIndex);
	//void updateDirection(bool isDoingAction, bool isJumping);
	//void updateGameEvent(VerticalDirection verticalState, unsigned int stateTime);
	//void updateAction(bool canAttack, VerticalDirection verticalState, Action action, unsigned int stateTime);

	void updateDirection();
	void updateVerticalDirection(Action action, VerticalDirection verticalDirection);
	void updateHorizontalDirection(Action action, HorizontalDirection horizontalDirection, VerticalDirection verticalDirection);
	void updateAction(HorizontalDirection horizontalDirection, VerticalDirection verticalDirection, Action action, unsigned int stateTime);
	void updateJump(Action action, VerticalDirection verticalDirection);
	void updateAttack(VerticalDirection verticalDirection, Action action);

	CharacterState mState;
	CharacterData* mData;

	unsigned int mInputs;
	unsigned int mJumpTimer;
	unsigned int mHitTimer;
	unsigned int mBlockTimer;
	GameInput mForwardDirection;
	GameInput mBackwardDirection;
	GameEvent mGameEvent;

	std::string moveSetString[25];
	std::string actionString[8];
};