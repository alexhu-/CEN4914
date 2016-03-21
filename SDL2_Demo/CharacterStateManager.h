#pragma once

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

private:
	void updateJump(bool isJumping, unsigned int stateTime);
	void updateAttack(bool canAttack, VerticalDirection verticalState);
	void setAttackAction(Action action);
	void updateAttackStatus(unsigned int attackIndex);
	void updateDirection(bool isDoingAction, bool isJumping);

	CharacterState mState;
	CharacterData* mData;

	unsigned int mInputs;
	unsigned int mJumpTimer;
};