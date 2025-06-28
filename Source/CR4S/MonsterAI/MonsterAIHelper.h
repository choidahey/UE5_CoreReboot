#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h" 

class UBehaviorTreeComponent;
class ABaseMonster;

namespace MonsterAIHelper
{
	ABaseMonster* GetControlledMonster(UBehaviorTreeComponent& OwnerComp);
	void SetMonstersHP(const UWorld* World, const float InHealth);
}