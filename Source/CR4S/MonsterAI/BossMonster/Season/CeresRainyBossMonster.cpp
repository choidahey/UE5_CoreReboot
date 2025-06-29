#include "MonsterAI/BossMonster/Season/CeresRainyBossMonster.h"

ACeresRainyBossMonster::ACeresRainyBossMonster()
{
	MonsterID = TEXT("CeresRainyBoss");
	EnvHumidDelta = 20.f;
	DropItems.Add({ FName("TearOfSorrow"), 1 });
}
