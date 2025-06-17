#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CR4S/MonsterAI/Data/MonsterEnum.h"
#include "MonsterStateComponent.generated.h"

struct FMonsterAttributeRow;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMonsterStateChanged, EMonsterState, PreviousState, EMonsterState,
                                             NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, EBossPhase, NewPhase);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CR4S_API UMonsterStateComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Constructor & Override

public:	
	UMonsterStateComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma endregion

#pragma region State Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Monster|State")
	void SetState(EMonsterState NewState);
	
	UFUNCTION(BlueprintCallable, Category = "Monster|State")
	FORCEINLINE EMonsterState GetCurrentState() const { return CurrentState; }
	
	UFUNCTION(BlueprintCallable, Category = "Monster|State")
	FORCEINLINE bool IsInState(EMonsterState State) const { return CurrentState == State; }

	UFUNCTION(BlueprintCallable, Category = "Monster|State")
	FORCEINLINE float GetTimeInCurrentState() const { return StateElapsedTime; }

	UFUNCTION(BlueprintCallable, Category = "Monster|State")
	float GetStateDuration(EMonsterState State) const;

	UFUNCTION(BlueprintCallable, Category = "Monster|State")
	void ForceInterrupt();

	/* Broadcast delegate when state changes */
	UPROPERTY(BlueprintAssignable, Category = "Monster|State")
	FOnMonsterStateChanged OnStateChanged;

#pragma endregion

#pragma region State Data

private:
	EMonsterState CurrentState;
	EMonsterState PreviousState;
	float StateElapsedTime;
	TMap<EMonsterState, float> StateDuration;

#pragma endregion

#pragma region Boss Phase

public:
	UPROPERTY(BlueprintReadOnly)
	EBossPhase CurrentPhase = EBossPhase::Normal;

	void SetPhase(EBossPhase NewPhase);
	EBossPhase GetCurrentPhase() const { return CurrentPhase; }

	UPROPERTY(BlueprintAssignable, Category = "Monster|State")
	FOnPhaseChanged OnPhaseChanged;

#pragma endregion

#pragma region Stun
public:
	UFUNCTION(BlueprintCallable, Category="Monster|State")
	void InitializeStunData(const FMonsterAttributeRow& Data);
	
	UFUNCTION(BlueprintCallable, Category = "Monster|State")
	void ApplyStun(float StunAmount);
    
	UFUNCTION(BlueprintCallable, Category = "Monster|State")
	void RemoveStunDebuff();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Monster|State")
	bool IsStunned() const { return bIsStunned; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Monster|State")
	float GetCurrentStunAmount() const { return MaxStun > 0.f ? CurrentStun / MaxStun : 0.f; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|State")
	float MaxStun = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|State")
	float StunDamageMultiplier = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|State")
	float StunChargeStartDelay = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|State")
	float StunRecoveryMin = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|State")
	float StunRecoveryMax = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|State")
	float StunRecoveryRampUpTime = 3.f;

	UPROPERTY(BlueprintReadOnly, Category = "Stun")
	float CurrentStun = 0.f;
    
	UPROPERTY(BlueprintReadOnly, Category = "Stun")
	bool bIsStunned = false;

private:
	FTimerHandle StunRecoveryTimerHandle;
	float RecoveryElapsedTime = 0.f;

#pragma endregion

private:
	FString MyHeader;

};
