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
const FName FAIKeys::bIsPlayingAttackMontage = TEXT("bIsPlayingAttackMontage");

// Region Boss AI Key
const FName FRegionBossAIKeys::CombatStartLocation = TEXT("CombatStartLocation");
const FName FRegionBossAIKeys::PatrolTargetLocation = TEXT("PatrolTargetLocation");
const FName FRegionBossAIKeys::CurrentPhase = TEXT("CurrentPhase");
const FName FRegionBossAIKeys::ShouldReturn = TEXT("ShouldReturn");
const FName FRegionBossAIKeys::LastIdleBoredTime = TEXT("LastIdleBoredTime");
const FName FRegionBossAIKeys::SkillTargetLocation = TEXT("SkillTargetLocation");
const FName FRegionBossAIKeys::CurrentPatternID = TEXT("CurrentPatternID");
const FName FRegionBossAIKeys::PreviousPatternID = TEXT("PreviousPatternID");
const FName FRegionBossAIKeys::PatternStepIndex = TEXT("PatternStepIndex");

// Season Boss AI Key
const FName FSeasonBossAIKeys::NearestHouseActor = TEXT("NearestHouseActor");
const FName FSeasonBossAIKeys::HouseAcceptRadius = TEXT("HouseAcceptRadius");
const FName FSeasonBossAIKeys::PlayerAcceptRadius = TEXT("PlayerAcceptRadius");
const FName FSeasonBossAIKeys::CurrentDistance = TEXT("CurrentDistance");
const FName FSeasonBossAIKeys::CurrentHP = TEXT("CurrentHP");
const FName FSeasonBossAIKeys::AttackType = TEXT("AttackType");
const FName FSeasonBossAIKeys::CurrentSkillRange = TEXT("CurrentSkillRange");
const FName FSeasonBossAIKeys::MeleeOptimalDistance = TEXT("MeleeOptimalDistance");
const FName FSeasonBossAIKeys::RangedOptimalDistance = TEXT("RangedOptimalDistance");
const FName FSeasonBossAIKeys::bIsInSkillRange = TEXT("bIsInSkillRange");
const FName FSeasonBossAIKeys::AggroTarget = TEXT("AggroTarget");
