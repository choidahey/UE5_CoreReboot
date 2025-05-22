#include "MonsterAIKeyNames.h"

// Common AI Monster Key
const FName FAIKeys::TargetActor = TEXT("TargetActor");
const FName FAIKeys::CanSeePlayer = TEXT("CanSeePlayer");
const FName FAIKeys::LastSeenLocation = TEXT("LastSeenLocation");
const FName FAIKeys::CanHearPlayer = TEXT("CanHearPlayer");
const FName FAIKeys::LastHeardLocation = TEXT("LastHeardLocation");
const FName FAIKeys::CurrentState = TEXT("CurrentState");
const FName FAIKeys::LastHitTime = TEXT("LastHitTime");
const FName FAIKeys::IsDead = TEXT("IsDead");
const FName FAIKeys::SkillIndex = TEXT("SkillIndex");

// Region Boss AI Key
const FName FRegionBossAIKeys::BaseCampLocation = TEXT("BaseCampLocation");
const FName FRegionBossAIKeys::CurrentPhase = TEXT("CurrentPhase");
const FName FRegionBossAIKeys::ShouldReturn = TEXT("ShouldReturn");

// Season Boss AI Key
const FName FSeasonBossAIKeys::InitializeTargetLocation = TEXT("InitializeTargetLocation");
const FName FSeasonBossAIKeys::NearestHouseLocation = TEXT("NearestHouseLocation");
