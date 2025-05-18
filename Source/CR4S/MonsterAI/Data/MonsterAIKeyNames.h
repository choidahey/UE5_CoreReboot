#pragma once

#include "CoreMinimal.h"

struct FAIKeys
{
	static const FName TargetActor;
	static const FName CanSeePlayer;
	static const FName LastSeenLocation;
	static const FName CanHearPlayer;
	static const FName LastHeardLocation;
	static const FName SearchState;
	static const FName IsDead;
};

struct FSeasonBossAIKeys : public FAIKeys
{
	static const FName InitializeTargetLocation;
	static const FName NearestHouseLocation;
};
