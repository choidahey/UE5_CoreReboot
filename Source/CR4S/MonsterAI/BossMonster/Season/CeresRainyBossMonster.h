#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "CeresRainyBossMonster.generated.h"

UCLASS()
class CR4S_API ACeresRainyBossMonster : public ASeasonBossMonster
{
	GENERATED_BODY()

public:
	ACeresRainyBossMonster();

protected:
	virtual UNiagaraSystem* GetOpeningNiagara() const override
	{
		return OpeningNiagara;
	}

	UPROPERTY(EditDefaultsOnly, Category="Boss|Opening Pattern")
	TObjectPtr<UNiagaraSystem> OpeningNiagara = nullptr;

private:
	FString MyHeader = TEXT("CeresRainyBossMonster");
};
