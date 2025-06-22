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
	static const FName bIsPlayingAttackMontage;
};

struct FRegionBossAIKeys : public FAIKeys
{
	static const FName CombatStartLocation;
	static const FName PatrolTargetLocation;
	static const FName CurrentPhase;
	static const FName ShouldReturn;
	static const FName LastIdleBoredTime;
	static const FName SkillTargetLocation;
	static const FName CurrentPatternID;
	static const FName PreviousPatternID;
	static const FName PatternStepIndex;
};

struct FSeasonBossAIKeys : public FAIKeys
{
	static const FName NearestHouseActor;
	static const FName HouseAcceptRadius;
	static const FName PlayerAcceptRadius;
	static const FName CurrentDistance;
	static const FName CurrentHP;
	static const FName AttackType;
	static const FName CurrentSkillRange;
	static const FName MeleeOptimalDistance;
	static const FName RangedOptimalDistance;
	static const FName bIsInSkillRange;
	static const FName AggroTarget;
	
	
};
