#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/BaseMonster.h"
#include "RegionBossMonster.generated.h"

class UNavigationInvokerComponent;

UCLASS()
class CR4S_API ARegionBossMonster : public ABaseMonster
{
	GENERATED_BODY()

#pragma region Constructor & Override

public:
	ARegionBossMonster();

#pragma endregion

#pragma region Navigation Invokers

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Component")
	float NavGenerationRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Component")
	float NavRemovalRadius;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	TObjectPtr<UNavigationInvokerComponent> NavInvoker;

#pragma endregion

#pragma region Return to BaseCamp
public:
	FORCEINLINE FVector GetBaseCampLocation() const { return BaseCampLocation;  }
	FORCEINLINE void SetBaseCampLocation(const FVector& Loc) { BaseCampLocation = Loc; }

public:
	FVector BaseCampLocation;

#pragma endregion

private:
	FString MyHeader;

};
