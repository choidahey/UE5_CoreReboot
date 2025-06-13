#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "VitaDryBossMonster.generated.h"

UCLASS()
class CR4S_API AVitaDryBossMonster : public ASeasonBossMonster
{
	GENERATED_BODY()

public:
	AVitaDryBossMonster();

protected:
	virtual UNiagaraSystem* GetOpeningNiagara() const override
	{
		return OpeningNiagara;
	}

	UPROPERTY(EditDefaultsOnly, Category="Boss|Opening Pattern")
	TObjectPtr<UNiagaraSystem> OpeningNiagara = nullptr;
};
