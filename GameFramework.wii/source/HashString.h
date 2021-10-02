#ifndef HashString_H
#define HashString_H

#include "HashLabel.h"

// pre-calculated hashes
//
// most of this is example only... just WiiMoteIdleTimeoutInSeconds is used in the frame work

class HashString
{
public:
	const static HashLabel BLANK;

	// menu names
	const static HashLabel Credits;
	const static HashLabel Start_Game;
	const static HashLabel Quit;
	const static HashLabel Intro;
	
	// fonts names
	const static HashLabel LargeFont;
	const static HashLabel SmallFont;

	// sound names
	const static HashLabel FireMissle;
	const static HashLabel AfterBurn;
	const static HashLabel DropMine;
	const static HashLabel Explode01;
	const static HashLabel Explode11;
	const static HashLabel hitmetal;

	// graphics names
	const static HashLabel ShipFrames;
	const static HashLabel AimingPointer;
	const static HashLabel MissileFrames;
	const static HashLabel Bad1Frames;
	const static HashLabel Bad2Frames;
	const static HashLabel BoomFrames;
	const static HashLabel Boom2Frames;
	const static HashLabel ThingFrames;
	const static HashLabel StarFrames;
	const static HashLabel ProbeMineFrames;
	const static HashLabel ProbeMineUpThrusterFrames;
	const static HashLabel ProbeMineRightThrusterFrames;
	const static HashLabel ProbeMineDownThrusterFrames;
	const static HashLabel ProbeMineLeftThrusterFrames;
	const static HashLabel SimpleMineFrames;
	const static HashLabel Boom3Frames;
	const static HashLabel Explosion64x64;
	const static HashLabel ShieldRed;
	const static HashLabel ShieldBlue;
	const static HashLabel Boom4Frames;
	const static HashLabel Boom5Frames;
	const static HashLabel Boom6Frames;

	// Variables
	const static HashLabel WiiMoteIdleTimeoutInSeconds;
	//const static HashLabel WaitTimeForGameCompletedMessage;
	const static HashLabel AmountStars;
	const static HashLabel AmountBadShips;
	const static HashLabel AmountBadSpores;
	const static HashLabel PlayerMaxShieldLevel;
	const static HashLabel ReducePlayerShieldLevelByAmountForShipToShipCollision;
	const static HashLabel BadShipType1MaxShieldLevel;
	const static HashLabel BadShipType2MaxShieldLevel;
	const static HashLabel ViewRadiusForSprites;
	const static HashLabel PlayerCollisionRadius;
	const static HashLabel PlayerShipCollisionFactor;
	const static HashLabel FoeShipCollisionFactor;
	const static HashLabel RocketCollisionRadius;
	const static HashLabel PlayerStartingPointX;
	const static HashLabel PlayerStartingPointY;
	const static HashLabel PlayerFacingDirection;
	const static HashLabel PlayerStartingVelocityX;
	const static HashLabel PlayerStartingVelocityY;
};


#endif
