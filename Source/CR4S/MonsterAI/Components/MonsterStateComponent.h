#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CR4S/MonsterAI/Data/MonsterEnum.h"
#include "MonsterStateComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMonsterStateChanged, EMonsterState, PreviousState, EMonsterState, NewState);

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

private:
	FString MyHeader;

};
