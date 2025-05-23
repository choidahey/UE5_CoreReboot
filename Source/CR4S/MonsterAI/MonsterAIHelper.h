#pragma once

#include "CoreMinimal.h"

class UBehaviorTreeComponent;
class ABaseMonster;

namespace MonsterAIHelper
{
	ABaseMonster* GetControlledMonster(UBehaviorTreeComponent& OwnerComp);
}