#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "CR4S/MonsterAI/Data/MonsterAttributeRow.h"
#include "CR4S/MonsterAI/Data/MonsterSkillData.h"
#include "MonsterDataSubsystem.generated.h"


UCLASS()
class CR4S_API UMonsterDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UMonsterDataSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

#pragma region AttributeData
	FMonsterAttributeRow* GetMonsterAttributeData(const FName MonsterID) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Monster Data")
	TObjectPtr<UDataTable> MonsterAttributeTable = nullptr;
#pragma endregion

#pragma region SkillData
public:
	UFUNCTION(BlueprintCallable, Category = "Monster Data")
	TArray<FMonsterSkillData> GetMonsterSkillData(const FName MonsterID) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Monster Data")
	TObjectPtr<UDataTable> MonsterSkillTable = nullptr;
#pragma endregion

private:
	FString MyHeader;
};
