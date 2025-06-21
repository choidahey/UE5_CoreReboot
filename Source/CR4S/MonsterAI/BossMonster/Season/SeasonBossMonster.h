#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/BaseMonster.h"
#include "SeasonBossMonster.generated.h"


class UNiagaraComponent;
class UNiagaraSystem;
class AEnvironmentalModifierVolume;
class UNavigationInvokerComponent;

DECLARE_MULTICAST_DELEGATE(FOnTeleportEndSignature);

UCLASS()
class CR4S_API ASeasonBossMonster : public ABaseMonster
{
	GENERATED_BODY()

public:
	ASeasonBossMonster();

protected:
	virtual void BeginPlay() override;
	virtual void HandleDeath() override;

#pragma region Opening Pattern
protected:
	void SpawnOpeningPattern();  // Template Method
	virtual UNiagaraSystem* GetOpeningNiagara() const PURE_VIRTUAL(ASeasonBossMonster::GetOpeningNiagara, return nullptr;);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening Pattern")
	float EnvVolRadius = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening Pattern")
	float EnvVolHeight = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening Pattern")
	float EnvVolChangeSpeed = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening Pattern")
	float EnvTempDelta = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Opening Pattern")
	float EnvHumidDelta = 0.f;

private:
	UPROPERTY()
	TObjectPtr<AEnvironmentalModifierVolume> SpawnedEnvVolume = nullptr;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SpawnedNiagaraComp = nullptr;
	
#pragma endregion

#pragma region NavInvoker
protected:
	UPROPERTY(EditAnywhere, Category = "MonsterAI")
	TObjectPtr<UNavigationInvokerComponent> NavInvoker;
	
	UPROPERTY(EditAnywhere, Category = "MonsterAI")
	float NavInvokerRadius = 5000.0f;
	UPROPERTY(EditAnywhere, Category = "MonsterAI")
	float NavInvokerRemovalRadius = 5500.f;

#pragma endregion
	
};
