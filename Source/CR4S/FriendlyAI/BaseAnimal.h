#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Game/Interface/Spawnable.h" // Added Spawnable Interface Library 
#include "Perception/AIPerceptionComponent.h"
#include "Engine/DataTable.h"
#include "Data/AnimalStatsRow.h"
#include "BaseAnimal.generated.h"

class UAnimalRangedAttackComponent;
class UAnimalPerceptionComponent;
class UGroundMovementComponent;
class UNavigationInvokerComponent;

UENUM(BlueprintType)
enum class EAnimalState : uint8
{
	Patrol,
	Chase,
	Attack,
	Flee,
	Perched,
	Stun,
	Dead,

	// Flying
	TakeOff,
	Cruise,
	Landing,

	// Monster
	AttackPrimaryTarget,  
	ReturnToPrimaryTarget 
};

UENUM(BlueprintType)
enum class EAnimalBehavior : uint8
{
	Aggressive,
	Coward,
	Passive_AggroOnHit,
	Passive_FleeOnHit
};

//DECLARE_MULTICAST_DELEGATE(FOnDied); 
//Declared in Spawnable Interface, thus Deleted 

UCLASS(Abstract)
class CR4S_API ABaseAnimal : public ACharacter, public ISpawnable //Interface Added
{
	GENERATED_BODY()

public:
	ABaseAnimal();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RowName;

protected:
	void LoadStats();
	
private:
	const FAnimalStatsRow* StatsRow = nullptr;
	FAnimalStatsRow CurrentStats;

public:
	const FAnimalStatsRow* GetStatsRowPtr() const { return StatsRow; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bStatsReady = false;
	
public:	
	UPROPERTY(BlueprintReadOnly)
	float StunValue = 0.0f;
	
	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	float JumpPower = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	AActor* CurrentTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	class USphereComponent* AttackRange;

	UPROPERTY(VisibleAnywhere)
	UGroundMovementComponent* GroundComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Navigation")
	UNavigationInvokerComponent* NavInvokerComponent;

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

	//Function to Return OnDied Delegate by Spawnable Interface
	virtual FOnDied* GetOnDiedDelegate() override { return &OnDied; }

public:
		
	UFUNCTION(BlueprintCallable)
	virtual void ApplyStun(float Amount);
	
	UFUNCTION(BlueprintCallable)
	virtual void RecoverFromStun();
	
	UFUNCTION(BlueprintCallable)
	void Die();
	
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UInteractableComponent> InteractableComponent;

	FTimerHandle StunRecoverTimer;
	
	bool bIsStunned = false;

	// void SetbIsTamed(bool bNewValue);

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

	//float LastAttackTime = 0.0f;
	//float CachedAttackInterval = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attack", meta=(AllowPrivateAccess="true"))
	UAnimalRangedAttackComponent* RangedAttackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attack", meta=(AllowPrivateAccess="true"))
	UArrowComponent* MuzzleArrow;


#pragma region Attack
	
public:
	// Cooldown Reset
	void ResetMeleeCooldown() { bIsMeleeOnCooldown = false; }
	void ResetChargeCooldown() { bIsChargeOnCooldown  = false; }
	void ResetRangedCooldown() { bIsRangedOnCooldown  = false; }
	
	virtual void PerformMeleeAttack();
	virtual void PerformChargeAttack();
	virtual void PerformRangedAttack();
	
	float PlayAttackMontage();
	float PlayChargeAttackMontage();
	float PlayRangedAttackMontage();

	void ResetAttackFlag() { bIsAttacking = false; }
	void ResetMeleeAttack() { bIsMeleeOnCooldown = false; }
	void ResetChargeAttack() { bIsChargeOnCooldown = false; }
	void ResetRangedAttack() { bIsRangedOnCooldown = false; }
	
public:	
	// Attack Montages
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* MeleeAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* ChargeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* RangedMontage;
	
	// CoolDown
	UPROPERTY(EditAnywhere, Category = "Attack Cooldown")
	float MeleeAttackCooldown = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Attack Cooldown")
	float ChargeAttackCooldown = 0.0f;
	
	UPROPERTY(EditAnywhere, Category = "Attack Cooldown")
	float RangedAttackCooldown = 0.0f;

	// CoolDown Timer
	FTimerHandle AttackTimerHandle;
	FTimerHandle MeleeAttackTimerHandle;
	FTimerHandle ChargeAttackTimerHandle;
	FTimerHandle RangedAttackTimerHandle;
	
	uint8 bIsAttacking : 1 = 0;
	
	// Has Attack
	uint8 bCanMelee : 1 = 0;
	uint8 bCanCharge : 1 = 0;
	uint8 bCanRanged : 1 = 0;
	
	// Cooldown State
	uint8 bIsMeleeOnCooldown : 1 = 0;
	uint8 bIsChargeOnCooldown : 1 = 0;
	uint8 bIsRangedOnCooldown : 1 = 0;

	// Attack Range
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float MeleeRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float DashRange = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float RangedRange = 0.0f;

	// Attack Probability
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float MeleeProbability = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float ChargeProbability = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float RangedProbability = 0.0f;

	
#pragma endregion

#pragma region Debug
public:
	void DrawDebugVisuals();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	uint8 bDrawSightDebug : 1 = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	uint8 bDrawAttackRangeDebug : 1 = 0;
#pragma endregion
};