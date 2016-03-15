#pragma once

enum MoveSet
{
	STAND_PUNCH1,
	STAND_PUNCH2,
	STAND_KICK1,
	STAND_KICK2,
	CROUCH_PUNCH1,
	CROUCH_PUNCH2,
	CROUCH_KICK1,
	CROUCH_KICK2,
	JUMP_PUNCH1,
	JUMP_PUNCH2,
	JUMP_KICK1,
	JUMP_KICK2,
	IDLE,
	WALK_FORWARD,
	WALK_BACKWARD,
	JUMP,
	CROUCH,
	STAND_BLOCK,
	CROUCH_BLOCK,
	STAND_HIT,
	CROUCH_HIT,
	JUMP_HIT,
	STAND_KNOCKDOWN,
	CROUCH_KNOCKDOWN
};

class CharacterData
{
public:
	CharacterData();
	// get the total startup frames for a specific move
	virtual unsigned int getTotalStartupFrames(unsigned int index);
	// get the total active frames for a specific move
	virtual unsigned int getTotalActiveFrames(unsigned int index);
	// get the total recovery frames for a specific move
	virtual unsigned int getTotalRecoveryFrames(unsigned int index);

	virtual unsigned int getTotalFrames(unsigned int index);

private:

};