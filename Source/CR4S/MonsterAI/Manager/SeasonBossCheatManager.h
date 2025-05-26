#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "SeasonBossCheatManager.generated.h"

UCLASS()
class CR4S_API USeasonBossCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Exec)
	void SetSeasonBossHealth(const FString& BossNameSubstring, float NewHealth);

    UFUNCTION(Exec)
    void HealSeasonBossHealth(const FString& MonsterID, float HealAmount);


    UFUNCTION(Exec)
    void DamageSeasonBossHealth(const FString& MonsterID, float DamageAmount);


    UFUNCTION(Exec)
    void KillMonster(const FString& MonsterID);
};
