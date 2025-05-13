#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Game/System/SeasonType.h"
#include "SeasonManager.generated.h"

UCLASS()
class CR4S_API USeasonManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void SetCurrentSeason(ESeasonType NewSeason);

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	void ChangeToNextSeason();

protected:
	void LoadSeason();

private:
	void HandleSeasonChange(ESeasonType NewSeason);

	ESeasonType CurrentSeason = ESeasonType::None;
	
};
