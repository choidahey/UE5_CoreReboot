#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "CR4S.h"
#include "NavigationInvokerComponent.h"

ASeasonBossMonster::ASeasonBossMonster()
{
   NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
   NavInvoker->SetGenerationRadii(NavInvokerRadius, NavInvokerRemovalRadius);
}

void ASeasonBossMonster::BeginPlay()
{
   Super::BeginPlay();
}