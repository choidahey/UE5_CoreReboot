#pragma once

#include "CoreMinimal.h"

struct FAIKeys
{
	static const FName TargetActor;
	static const FName CanSeePlayer;
	static const FName LastSeenLocation;
	static const FName CanHearPlayer;
	static const FName LastHeardLocation;
	static const FName CurrentState;
	static const FName LastHitTime;
	static const FName IsDead;
	static const FName SkillIndex;
};

struct FRegionBossAIKeys : public FAIKeys
{
	static const FName BaseCampLocation;
	static const FName CurrentPhase;
	static const FName ShouldReturn;
};

struct FSeasonBossAIKeys : public FAIKeys
{
	static const FName InitializeTargetLocation;
	static const FName NearestHouseActor;
};
