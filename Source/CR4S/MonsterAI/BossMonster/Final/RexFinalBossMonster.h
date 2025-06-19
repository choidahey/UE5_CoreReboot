#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "RexFinalBossMonster.generated.h"

UCLASS()
class CR4S_API ARexFinalBossMonster : public ASeasonBossMonster
{
	GENERATED_BODY()

public:
	ARexFinalBossMonster();

protected:
	virtual void BeginPlay() override;

	virtual UNiagaraSystem* GetOpeningNiagara() const override
	{
		return OpeningNiagara;
	}

	UPROPERTY(EditDefaultsOnly, Category="Boss|Opening Pattern")
	TObjectPtr<UNiagaraSystem> OpeningNiagara = nullptr;
};
