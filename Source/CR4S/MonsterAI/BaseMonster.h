#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Game/Interface/Spawnable.h"
#include "Data/MonsterEnum.h"
#include "BaseMonster.generated.h"

class UMonsterAttributeComponent;
class UMonsterSkillComponent;
class UMonsterStateComponent;
class UMonsterAnimComponent;
class UBehaviorTree;

UCLASS()
class CR4S_API ABaseMonster : public ACharacter, public ISpawnable
{
	GENERATED_BODY()


#pragma region ACharacter Override

public:
	ABaseMonster();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

#pragma endregion

#pragma region BaseMonster Components
 
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	TObjectPtr<UMonsterAttributeComponent> AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	TObjectPtr<UMonsterSkillComponent> SkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	TObjectPtr<UMonsterStateComponent> StateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	TObjectPtr<UMonsterAnimComponent> AnimComponent;

#pragma endregion

#pragma region Monster AI

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

#pragma endregion

#pragma region SKill Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	virtual void UseSkill(int32 SkillIndex);

	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	virtual int32 SelectSkillIndex();

#pragma endregion

#pragma region Monster Behavior - Death

public:
	UFUNCTION(BlueprintCallable, Category = "Monster|State")
	bool IsDead() const;

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|State")
	bool bIsDead = false;

#pragma endregion

#pragma region Monster Components
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Monster|Attribute")
	FName MonsterID;

#pragma endregion

#pragma region Monster Event Handler

protected:
	/* Called when monster dies */
	UFUNCTION()
	virtual void HandleDeath();

	/* Called when monster state changes */
	UFUNCTION()
	virtual void OnMonsterStateChanged(EMonsterState Previous, EMonsterState Current);

	//Function to Return OnDied Delegate by Spawnable Interface
	virtual FOnDied* GetOnDiedDelegate() override { return &OnDied; }

	// Delegate Variable
	FOnDied OnDied;

#pragma endregion

#pragma region Accumulate Damage

public:
	FORCEINLINE float GetAccumulatedDamage() const { return AccumulatedDamage; }
	FORCEINLINE void AddAccumulatedDamage(float Damage) { AccumulatedDamage += Damage; }
	FORCEINLINE void ClearAccumulatedDamage() { AccumulatedDamage = 0.f; }

private:
	UPROPERTY()
	float AccumulatedDamage = 0.f;

#pragma endregion

private:
	FString MyHeader;

};
