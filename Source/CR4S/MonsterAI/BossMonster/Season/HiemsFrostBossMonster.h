#pragma once
#include "CoreMinimal.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "HiemsFrostBossMonster.generated.h"


UCLASS()
class CR4S_API AHiemsFrostBossMonster : public ASeasonBossMonster
{
	GENERATED_BODY()

public:
	AHiemsFrostBossMonster();

protected:
	virtual UNiagaraSystem* GetOpeningNiagara() const override { return OpeningNiagara; }
	virtual USoundBase* GetBattleBGM() const override{ return BattleBGM; }

	UPROPERTY(EditDefaultsOnly, Category="Opening Pattern")
	TObjectPtr<UNiagaraSystem> OpeningNiagara = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="Opening Pattern")
	TObjectPtr<USoundBase> BattleBGM = nullptr;
	
private:
	FString MyHeader = TEXT("HiemsFrostBossMonster");
};
