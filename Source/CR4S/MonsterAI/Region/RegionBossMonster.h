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

protected:
	virtual void BeginPlay() override;

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

#pragma region Combat Info
public:
	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	FORCEINLINE void SetCombatStartLocation() { CombatStartLocation = GetActorLocation(); }
	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	FORCEINLINE FVector GetCombatStartLocation() const { return CombatStartLocation; }

	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	virtual bool IsOutsideCombatRange(float Tolerance = 0.f) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Combat")
	FVector CombatStartLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Combat")
	float CombatRange = 4000.f;

#pragma endregion

private:
	FString MyHeader;

};
