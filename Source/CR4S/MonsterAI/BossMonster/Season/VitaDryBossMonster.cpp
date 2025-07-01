#include "MonsterAI/BossMonster/Season/VitaDryBossMonster.h"

AVitaDryBossMonster::AVitaDryBossMonster()
{
	MonsterID = TEXT("VitaDryBoss");
	EnvTempDelta = 10.f;
	DropItems.Add({ FName("HeartOfTheSun"), 1 });
}
