#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CR4S/MonsterAI/Data/MonsterSkillRow.h"
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

#pragma endregion

#pragma region Skill Usage
public:
	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	void PlayPreMontage(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	void UseSkill(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	bool IsSkillReady(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	const FMonsterSkillRow GetSkillData(int32 Index) const;

#pragma endregion

#pragma region Helpers
private:
	UAnimInstance* GetAnimInstance() const;
	void ResetCooldown(int32 Index);

#pragma endregion

#pragma region Properties
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Skill")
	TArray<FMonsterSkillRow> SkillList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Skill")
	TArray<bool> bSkillReady;

protected:
	TArray<FTimerHandle> SkillCooldownTimers;

#pragma endregion
		
private:
	FString MyHeader;

};
