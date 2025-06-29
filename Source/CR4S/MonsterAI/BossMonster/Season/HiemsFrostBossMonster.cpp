#include "MonsterAI/BossMonster/Season/HiemsFrostBossMonster.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

AHiemsFrostBossMonster::AHiemsFrostBossMonster()
{
	MonsterID = TEXT("HiemsFrostBoss");
	EnvTempDelta = -10.f;
	DropItems.Add({ FName("IceCrystal"), 1 });
}