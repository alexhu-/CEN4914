#pragma once

#include "Model.h"

// start active recovery end frames in terms of original 24fps
// total frame duration in 60fps
struct ModelAction
{
	int start;
	int active;
	int recovery;
	int end;

	int totalStartup;
	int totalActive;
	int totalRecovery;

	ModelAction() :
		start(0),
		active(0),
		recovery(0),
		end(0),
		totalStartup(0),
		totalActive(0),
		totalRecovery(0)
	{

	}

	// Holds data about the model's startup, active, recovery, and end frames
	// these frames are from the model itself
	// total startup, total active, and total recovery frames are set by the user
	// and can be change any time
	// Example: in the model, punch1 starts at frame 181, has active frame starting at 186
	// recovery frames at 188, and ends on frame 192 in the animation file
	// And the user wants the startup of punch to be 5 frames, active for 5 frames and recovers in 5 frames
	// so total startup.. etc would be 5/5/5
	ModelAction(int s, int a, int r, int e, int totals, int totala, int totalr) :
		start(s),
		active(a),
		recovery(r),
		end(e),
		totalStartup(totals),
		totalActive(totala),
		totalRecovery(totalr)
	{

	}
};

// Class with hardcoded values to separate each animation of the model
// Assumes the game will be running at 60 fps
class MyAnimatedMeshClass
{
public:
	MyAnimatedMeshClass();
	unsigned int getMaxAnimations();
	void setAnimationIndex(unsigned int index);
	float getAnimationTime(float timeMS);

private:
	void standAttackSetup(); // index 0 to 3
	void crouchAttackSetup(); // index 4 to 7
	void jumpAttackSetup(); // index 8 to 11

	void movementSetup(); // index 12 to 16
	void blockSetup(); // index 17 to 18
	void hitSetup(); // index 19 to 21
	void knockdownSetup(); // index 22 to 23

	ModelAction mActions[24];
	
	const unsigned int mAnimationCount = 24; // 27 no fire ball, default, and throw whiff
	unsigned int mAnimationIndex;
	double mInternalTimeMS;
	double mPrevTimeMS;
};