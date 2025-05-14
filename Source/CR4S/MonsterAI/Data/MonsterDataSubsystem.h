#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CR4S/MonsterAI/Data/MonsterAttributeRow.h"
#include "CR4S/MonsterAI/Data/MonsterSkillRow.h"
#include "MonsterDataSubsystem.generated.h"


UCLASS()
class CR4S_API UMonsterDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

#pragma region AttributeData
	const FMonsterAttributeRow* GetMonsterAttributeData(FName MonsterID) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Monster Data")
	TSoftObjectPtr<UDataTable> MonsterAttributeTableAsset;
	UDataTable* MonsterAttributeTable = nullptr;
#pragma endregion

#pragma region SkillData
public:
	const FMonsterSkillRow* GetMonsterSkillData(FName SkillID) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Monster Data")
	TSoftObjectPtr<UDataTable> MonsterSkillTableAsset;
	UDataTable* MonsterSkillTable = nullptr;
#pragma endregion
};
