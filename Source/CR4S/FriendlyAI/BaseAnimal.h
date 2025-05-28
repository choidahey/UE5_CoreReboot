#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Engine/DataTable.h"
#include "Data/AnimalStatsRow.h"
#include "BaseAnimal.generated.h"

UENUM(BlueprintType)
enum class EAnimalState : uint8
{
	Patrol,
	Chase,
	Attack,
	Flee,
	Stun,
	Dead
};

UENUM(BlueprintType)
enum class EAnimalBehavior : uint8
{
	Aggressive,
	Coward,
	Passive_AggroOnHit,
	Passive_FleeOnHit
};

DECLARE_MULTICAST_DELEGATE(FOnDied);

UCLASS(Abstract)
class CR4S_API ABaseAnimal : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseAnimal();

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RowName;

private:
	const FAnimalStatsRow* StatsRow = nullptr;
	FAnimalStatsRow CurrentStats;
	void LoadStats();

public:
	const FAnimalStatsRow* GetStatsRowPtr() const { return StatsRow; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bStatsReady = false;
	
public:	
	UPROPERTY(BlueprintReadOnly)
	float StunValue;
	
	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly)
	AActor* CurrentTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	class USphereComponent* AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	bool bIsFemale = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tame")
	bool bIsTamed = false;

	UPROPERTY(BlueprintReadOnly)
	EAnimalState CurrentState = EAnimalState::Patrol;
	
	UPROPERTY(BlueprintReadOnly)
	EAnimalBehavior BehaviorTypeEnum;

	const FAnimalStatsRow& GetCurrentStats() const { return CurrentStats; }
	void SetAnimalState(EAnimalState NewState);
	void ClearTarget();

	FOnDied OnDied;

public:
	UFUNCTION(BlueprintCallable)
	void MoveToLocation(const FVector& Dest);
	
	UFUNCTION(BlueprintCallable)
	void PerformAttack();
	
	UFUNCTION(BlueprintCallable)
	void ApplyStun(float Amount);
	
	UFUNCTION(BlueprintCallable)
	void Die();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* AttackMontage;

	UPROPERTY(VisibleAnywhere)
	bool bIsAttacking = false;

	void PlayAttackMontage();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInteractableComponent> InteractableComponent;

	FTimerHandle StunRecoverTimer;
	bool bIsStunned = false;

	void RecoverFromStun();

	void SetbIsTamed(bool bNewValue);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UAnimalInteractWidget> InteractWidgetClass;

	UFUNCTION()
	void OnInteract(AActor* Interactor);

	UPROPERTY()
	TObjectPtr<class UAnimalInteractWidget> ActiveInteractWidget;

	UFUNCTION()
	void Capture();

	UFUNCTION()
	void Butcher();

	float LastAttackTime = 0.0f;
	float CachedAttackInterval = 0.0f;
};