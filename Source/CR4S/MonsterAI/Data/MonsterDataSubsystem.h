#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CR4S/MonsterAI/Data/MonsterAttributeRow.h"
#include "CR4S/MonsterAI/Data/MonsterSkillData.h"
#include "MonsterDataSubsystem.generated.h"


UCLASS()
class CR4S_API UMonsterDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

#pragma region AttributeData
	FMonsterAttributeRow* GetMonsterAttributeData(FName MonsterID) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Monster Data")
	TSoftObjectPtr<UDataTable> MonsterAttributeTable;
#pragma endregion

#pragma region SkillData
public:
	const TArray<FMonsterSkillData>& GetMonsterSkillData(FName MonsterID) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Monster Data")
	UDataTable* MonsterSkillTable = nullptr;

	TMap<FName, TArray<FMonsterSkillData>> CacheSkillData;

private:
	void CacheMonsterSkillData();
#pragma endregion
};
