#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/BaseMonster.h"
#include "RegionBossMonster.generated.h"

class UNavigationInvokerComponent;

UCLASS()
class CR4S_API ARegionBossMonster : public ABaseMonster
{
	GENERATED_BODY()

#pragma region Constructor

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

public:
	FVector BaseCampLocation;

#pragma endregion

#pragma region Bored Montages

protected:
	UFUNCTION()
	void OnIdleBoredNotify();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Idle")
	TArray<TObjectPtr<UAnimMontage>> IdleBoredMontages;

#pragma endregion

private:
	FString MyHeader;

};
