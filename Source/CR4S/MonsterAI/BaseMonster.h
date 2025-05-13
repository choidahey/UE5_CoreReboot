#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/MonsterEnum.h"
#include "BaseMonster.generated.h"

class UMonsterAttributeComponent;
class UMonsterSkillComponent;
class UMonsterStateComponent;
class UMonsterPerceptionComponent;
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
 
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	UMonsterAttributeComponent* AttributeComp;

//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
//	UMonsterSkillComponent* SkillComp;
//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	UMonsterStateComponent* StateComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	UMonsterPerceptionComponent* PerceptionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	UMonsterAnimComponent* AnimComp;

#pragma endregion

#pragma region Monster AI

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|AI")
	UBehaviorTree* BehaviorTree;

#pragma endregion

#pragma region Monster Behavior - Combat

public:
	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	virtual void TryAttack();
	
	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	virtual void PerfromAttack(int32 AttackIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Monster|Combat")
	virtual bool CanAttack() const;

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

#pragma region Monster Behavior - Targeting

public:
	UFUNCTION(BlueprintCallable, Category = "Monster|Target")
	virtual void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Monster|Target")
	FORCEINLINE AActor* GetTargetActor() const { return TargetActor; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Target")
	AActor* TargetActor;

#pragma endregion

#pragma region Monster Components - Attribute
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

	/* Called when target is detected via perception */
	UFUNCTION()
	virtual void OnTargetDetected(AActor* DetectedActor);

	/* Called when target is lost via perception */
	UFUNCTION()
	virtual void OnTargetLost(AActor* LostActor);

#pragma endregion

private:
	FString MyHeader;

};
