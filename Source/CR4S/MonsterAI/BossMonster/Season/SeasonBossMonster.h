#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/BaseMonster.h"
#include "SeasonBossMonster.generated.h"


class UNavigationInvokerComponent;

UCLASS()
class CR4S_API ASeasonBossMonster : public ABaseMonster
{
	GENERATED_BODY()

public:
	ASeasonBossMonster();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "MonsterAI")
	UNavigationInvokerComponent* NavInvoker;

private:
	float NavInvokerRadius = 2000.0f;
	float NavInvokerRemovalRadius = 2500.f;

	FString MyHeader = TEXT("SeasonMonster");
};
