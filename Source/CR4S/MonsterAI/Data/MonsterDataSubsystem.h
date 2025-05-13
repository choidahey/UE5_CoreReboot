#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CR4S/MonsterAI/Data/MonsterAttributeRow.h"
#include "MonsterDataSubsystem.generated.h"


UCLASS()
class CR4S_API UMonsterDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	const FMonsterAttributeRow* GetMonsterAttributeRow(FName MonsterID) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Monster Data")
	TSoftObjectPtr<UDataTable> MonsterAttributeTableAsset;

	UDataTable* MonsterAttributeTable = nullptr;
};
