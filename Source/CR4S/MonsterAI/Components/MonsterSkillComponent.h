#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CR4S/MonsterAI/Data/MonsterSkillData.h"
#include "MonsterSkillComponent.generated.h"

class USkillDataSubsystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CR4S_API UMonsterSkillComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Constructor
public:	
	UMonsterSkillComponent();

#pragma endregion

#pragma region Initialize
public:
	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	void InitializeMonsterSkills(const FName MonsterID);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma endregion

#pragma region Skill Usage
public:
	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	void UseSkill(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	bool IsSkillReady(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	float GetSkillRange(int32 Index) const;

	UFUNCTION(BlueprintCallable)
	const FMonsterSkillData& GetCurrentSkillData() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Monster|Skill")
	int32 CurrentSkillIndex = INDEX_NONE;

#pragma endregion

#pragma region Helpers
private:
	UAnimInstance* GetAnimInstance() const;
	void ResetCooldown(int32 Index);

#pragma endregion

#pragma region Properties
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Skill")
	TArray<FMonsterSkillData> SkillList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Skill")
	TArray<bool> bSkillReady;

protected:
	TArray<FTimerHandle> SkillCooldownTimers;

#pragma endregion
		
private:
	FString MyHeader;

};
