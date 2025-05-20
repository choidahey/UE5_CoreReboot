#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/MonsterEnum.h"
#include "BaseMonster.generated.h"

class UMonsterAttributeComponent;
class UMonsterSkillComponent;
class UMonsterStateComponent;
class UMonsterAnimComponent;
class UBehaviorTree;

UCLASS()
class CR4S_API ABaseMonster : public ACharacter
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
	void PlayPreMontage(int32 SkillIndex);

	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	virtual void UseSkill(int32 SkillIndex);

	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	virtual int32 SelectSkillIndex();

#pragma endregion

#pragma region Monster Behavior - Death

public:

	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Monster|State")
	bool IsDead() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|State")
	bool bIsDead = false;

#pragma endregion

#pragma region Monster Components
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Monster|Attribute")
	FName MonsterID;

	UFUNCTION(BlueprintCallable, Category = "Monster|Attribute")
	FORCEINLINE UMonsterAttributeComponent* GetAttributeComponent() const { return AttributeComponent; }

	UFUNCTION(BlueprintCallable, Category = "Monster|Skill")
	FORCEINLINE UMonsterSkillComponent* GetSkillComponent() const { return SkillComponent; }

	UFUNCTION(BlueprintCallable, Category = "Monster|State")
	FORCEINLINE UMonsterStateComponent* GetStateComponent() const { return StateComponent; }

	UFUNCTION(BlueprintCallable, Category = "Monster|Anim")
	FORCEINLINE UMonsterAnimComponent* GetAnimComponent() const { return AnimComponent; }

#pragma endregion

#pragma region Monster Event Handler

protected:
	/* Called when monster dies */
	UFUNCTION()
	virtual void HandleDeath();

	/* Called when monster state changes */
	UFUNCTION()
	virtual void OnMonsterStateChanged(EMonsterState Previous, EMonsterState Current);

#pragma endregion

private:
	FString MyHeader;

};
