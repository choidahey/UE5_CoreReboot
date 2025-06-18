#include "MonsterAI/BossMonster/Final/RexFinalBossMonster.h"
#include "Game/System/SeasonManager.h"

ARexFinalBossMonster::ARexFinalBossMonster()
{
	MonsterID = TEXT("RexFinalBoss");
}

void ARexFinalBossMonster::BeginPlay()
{
	Super::BeginPlay();

	if (USeasonManager* SeasonManager = GetWorld()->GetSubsystem<USeasonManager>())
	{
		switch (ESeasonType Season = SeasonManager->GetCurrentSeason())
		{
		case ESeasonType::BountifulSeason:
			EnvTempDelta = 10.f;
			EnvHumidDelta = 10.f;
			break;
		case ESeasonType::FrostSeason:
			EnvTempDelta = -15.f;
			break;
		case ESeasonType::RainySeason:
			EnvHumidDelta = 25.f;
			break;
		case ESeasonType::DrySeason:
			EnvTempDelta = 15.f;
			break;
		default:
			break;
		}
	}
}


