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
	
	UPROPERTY(EditAnywhere, Category = "MonsterAI")
	float NavInvokerRadius = 5000.0f;
	
	UPROPERTY(EditAnywhere, Category = "MonsterAI")
	float NavInvokerRemovalRadius = 5500.f;

private:
	FString MyHeader = TEXT("SeasonMonster");
};
