#include "MyAnimatedMeshClass.h"

MyAnimatedMeshClass::MyAnimatedMeshClass() :
	mAnimationIndex(0),
	mInternalTimeMS(0.0),
	mPrevTimeMS(0.0)
{
	standAttackSetup();
	crouchAttackSetup();
	jumpAttackSetup();
	movementSetup(); // index 12 to 16
	blockSetup(); // index 17 to 18
	hitSetup(); // index 19 to 21
	knockdownSetup(); // index 22 to 23
}

unsigned int MyAnimatedMeshClass::getMaxAnimations()
{
	return mAnimationCount;
}

void MyAnimatedMeshClass::setAnimationIndex(unsigned int index)
{
	if (index >= mAnimationCount)
	{
		return;
	}

	mAnimationIndex = index;
	mInternalTimeMS = 0.0;
}

float MyAnimatedMeshClass::getAnimationTime(float timeMS)
{
	int totalFrames = (mActions[mAnimationIndex].end - mActions[mAnimationIndex].start);

	double timeActionStart = mActions[mAnimationIndex].totalStartup * 1000.0 / 60.0;
	double timeActionActive = timeActionStart + mActions[mAnimationIndex].totalActive * 1000.0 / 60.0;
	double timeActionRecover = timeActionActive + (mActions[mAnimationIndex].totalRecovery - 1) * 1000.0 / 60.0;

	double ratio = 1.0;
	double originalFrame = 0.0;
	if (mInternalTimeMS < timeActionStart)
	{
		ratio = mInternalTimeMS / (timeActionStart);
		originalFrame = ratio * (mActions[mAnimationIndex].active - mActions[mAnimationIndex].start);
	}
	else if (mInternalTimeMS < timeActionActive)
	{
		ratio = (mInternalTimeMS - timeActionStart) / (timeActionActive - timeActionStart);
		originalFrame = ratio * (mActions[mAnimationIndex].recovery - mActions[mAnimationIndex].active) + (mActions[mAnimationIndex].active - mActions[mAnimationIndex].start);
	}
	else
	{
		ratio = (mInternalTimeMS - timeActionActive) / (timeActionRecover - timeActionActive);
		originalFrame = ratio * (mActions[mAnimationIndex].end - mActions[mAnimationIndex].recovery) + (mActions[mAnimationIndex].recovery - mActions[mAnimationIndex].start);
	}

	mInternalTimeMS += timeMS;//(timeMS - mPrevTimeMS);

	if (mInternalTimeMS > timeActionRecover)
	{
		mInternalTimeMS = 0.0;
	}
	//mPrevTimeMS = timeMS;

	if (originalFrame > totalFrames)
	{
		originalFrame = totalFrames;
	}

	return (float)((float)(originalFrame + mActions[mAnimationIndex].start) * 1000.0 / 24.0);
}

void MyAnimatedMeshClass::standAttackSetup()
{
	ModelAction* pModelAction = &mActions[0];

	// punch1
	pModelAction->start = 181;
	pModelAction->active = 186;
	pModelAction->recovery = 188;
	pModelAction->end = 192;
	pModelAction->totalStartup = 3;
	pModelAction->totalActive = 2;
	pModelAction->totalRecovery = 7;

	pModelAction = &mActions[1];

	// punch2
	pModelAction->start = 192;
	pModelAction->active = 198;
	pModelAction->recovery = 200;
	pModelAction->end = 204;
	pModelAction->totalStartup = 8;
	pModelAction->totalActive = 4;
	pModelAction->totalRecovery = 18;

	pModelAction = &mActions[2];

	// kick1
	pModelAction->start = 204;
	pModelAction->active = 210;
	pModelAction->recovery = 212;
	pModelAction->end = 216;
	pModelAction->totalStartup = 5;
	pModelAction->totalActive = 3;
	pModelAction->totalRecovery = 10;

	pModelAction = &mActions[3];

	// kick2
	pModelAction->start = 216;
	pModelAction->active = 222;
	pModelAction->recovery = 224;
	pModelAction->end = 228;
	pModelAction->totalStartup = 12;
	pModelAction->totalActive = 6;
	pModelAction->totalRecovery = 22;
}

void MyAnimatedMeshClass::crouchAttackSetup()
{
	ModelAction* pModelAction = &mActions[4];

	// punch1
	pModelAction->start = 229;
	pModelAction->active = 235;
	pModelAction->recovery = 237;
	pModelAction->end = 240;
	pModelAction->totalStartup = 4;
	pModelAction->totalActive = 2;
	pModelAction->totalRecovery = 6;

	pModelAction = &mActions[5];

	// punch2
	pModelAction->start = 240;
	pModelAction->active = 245;
	pModelAction->recovery = 248;
	pModelAction->end = 252;
	pModelAction->totalStartup = 18;
	pModelAction->totalActive = 7;
	pModelAction->totalRecovery = 21;

	pModelAction = &mActions[6];

	// kick1
	pModelAction->start = 252;
	pModelAction->active = 258;
	pModelAction->recovery = 261;
	pModelAction->end = 264;
	pModelAction->totalStartup = 5;
	pModelAction->totalActive = 4;
	pModelAction->totalRecovery = 8;

	pModelAction = &mActions[7];

	// kick2
	pModelAction->start = 264;
	pModelAction->active = 268;
	pModelAction->recovery = 272;
	pModelAction->end = 276;
	pModelAction->totalStartup = 7;
	pModelAction->totalActive = 8;
	pModelAction->totalRecovery = 26;
}

void MyAnimatedMeshClass::jumpAttackSetup()
{
	ModelAction* pModelAction = &mActions[8];

	// punch1
	pModelAction->start = 277;
	pModelAction->active = 282;
	pModelAction->recovery = 284;
	pModelAction->end = 288;
	pModelAction->totalStartup = 4;
	pModelAction->totalActive = 6;
	pModelAction->totalRecovery = 50;

	pModelAction = &mActions[9];

	// punch2
	pModelAction->start = 288;
	pModelAction->active = 294;
	pModelAction->recovery = 296;
	pModelAction->end = 300;
	pModelAction->totalStartup = 11;
	pModelAction->totalActive = 12;
	pModelAction->totalRecovery = 50;

	pModelAction = &mActions[10];

	// kick1
	pModelAction->start = 300;
	pModelAction->active = 306;
	pModelAction->recovery = 308;
	pModelAction->end = 312;
	pModelAction->totalStartup = 5;
	pModelAction->totalActive = 4;
	pModelAction->totalRecovery = 50;

	pModelAction = &mActions[11];

	// kick2
	pModelAction->start = 312;
	pModelAction->active = 318;
	pModelAction->recovery = 321;
	pModelAction->end = 324;
	pModelAction->totalStartup = 9;
	pModelAction->totalActive = 8;
	pModelAction->totalRecovery = 50;
}

void MyAnimatedMeshClass::movementSetup()
{
	ModelAction* pModelAction = &mActions[12];

	// idle
	pModelAction->start = 1;
	pModelAction->active = 8;
	pModelAction->recovery = 16;
	pModelAction->end = 24;
	pModelAction->totalStartup = 8;
	pModelAction->totalActive = 16;
	pModelAction->totalRecovery = 8;

	pModelAction = &mActions[13];

	// walk forward
	pModelAction->start = 24;
	pModelAction->active = 28;
	pModelAction->recovery = 32;
	pModelAction->end = 36;
	pModelAction->totalStartup = 8;
	pModelAction->totalActive = 8;
	pModelAction->totalRecovery = 8;

	pModelAction = &mActions[14];

	// walk backward
	pModelAction->start = 36;
	pModelAction->active = 40;
	pModelAction->recovery = 44;
	pModelAction->end = 48;
	pModelAction->totalStartup = 8;
	pModelAction->totalActive = 8;
	pModelAction->totalRecovery = 8;

	pModelAction = &mActions[15];

	// jump
	pModelAction->start = 48;
	pModelAction->active = 51;
	pModelAction->recovery = 66;
	pModelAction->end = 72;
	pModelAction->totalStartup = 3;
	pModelAction->totalActive = 53;
	pModelAction->totalRecovery = 4;

	pModelAction = &mActions[16];

	// crouch
	pModelAction->start = 83;//72;
	pModelAction->active = 84;//80;
	pModelAction->recovery = 85;//88;
	pModelAction->end = 86;//96;
	pModelAction->totalStartup = 8;
	pModelAction->totalActive = 8;
	pModelAction->totalRecovery = 8;
}

void MyAnimatedMeshClass::blockSetup()
{
	ModelAction* pModelAction = &mActions[17];

	// stand block
	pModelAction->start = 96;
	pModelAction->active = 100;
	pModelAction->recovery = 104;
	pModelAction->end = 108;
	pModelAction->totalStartup = 8;
	pModelAction->totalActive = 28;
	pModelAction->totalRecovery = 8;

	pModelAction = &mActions[18];

	// crouch block
	pModelAction->start = 109;
	pModelAction->active = 112;
	pModelAction->recovery = 116;
	pModelAction->end = 120;
	pModelAction->totalStartup = 8;
	pModelAction->totalActive = 28;
	pModelAction->totalRecovery = 8;
}

void MyAnimatedMeshClass::hitSetup()
{
	ModelAction* pModelAction = &mActions[19];

	// stand hit
	pModelAction->start = 121;
	pModelAction->active = 124;
	pModelAction->recovery = 128;
	pModelAction->end = 132;
	pModelAction->totalStartup = 8;
	pModelAction->totalActive = 28;
	pModelAction->totalRecovery = 8;

	pModelAction = &mActions[20];

	// crouch hit
	pModelAction->start = 133;
	pModelAction->active = 136;
	pModelAction->recovery = 140;
	pModelAction->end = 144;
	pModelAction->totalStartup = 8;
	pModelAction->totalActive = 28;
	pModelAction->totalRecovery = 8;

	pModelAction = &mActions[21];

	// jump hit
	pModelAction->start = 145;
	pModelAction->active = 148;
	pModelAction->recovery = 152;
	pModelAction->end = 156;
	pModelAction->totalStartup = 8;
	pModelAction->totalActive = 28;
	pModelAction->totalRecovery = 8;
}

void MyAnimatedMeshClass::knockdownSetup()
{
	ModelAction* pModelAction = &mActions[22];

	// stand knockdown
	pModelAction->start = 157;
	pModelAction->active = 162;
	pModelAction->recovery = 162;
	pModelAction->end = 168;
	pModelAction->totalStartup = 4;
	pModelAction->totalActive = 56;
	pModelAction->totalRecovery = 4;

	pModelAction = &mActions[23];

	// crouch knockdown
	pModelAction->start = 169;
	pModelAction->active = 174;
	pModelAction->recovery = 174;
	pModelAction->end = 180;
	pModelAction->totalStartup = mActions[22].totalStartup;
	pModelAction->totalActive = mActions[22].totalActive;
	pModelAction->totalRecovery = mActions[22].totalStartup;

	pModelAction = &mActions[24];

	// jump knockdown
	pModelAction->start = 162;
	pModelAction->active = 163;
	pModelAction->recovery = 163;
	pModelAction->end = 168;
	pModelAction->totalStartup = 1;
	pModelAction->totalActive = mActions[22].totalActive + 3;
	pModelAction->totalRecovery = mActions[22].totalStartup;
}

unsigned int MyAnimatedMeshClass::getTotalStartupFrames(unsigned int index)
{
	if (index < mAnimationCount)
	{
		return mActions[index].totalStartup;
	}
}

unsigned int MyAnimatedMeshClass::getTotalActiveFrames(unsigned int index)
{
	if (index < mAnimationCount)
	{
		return mActions[index].totalActive;
	}
}

unsigned int MyAnimatedMeshClass::getTotalRecoveryFrames(unsigned int index)
{
	if (index < mAnimationCount)
	{
		return mActions[index].totalRecovery;
	}
}

unsigned int MyAnimatedMeshClass::getTotalFrames(unsigned int index)
{
	if (index < mAnimationCount)
	{
		return mActions[index].totalRecovery + mActions[index].totalActive + mActions[index].totalStartup;
	}
}