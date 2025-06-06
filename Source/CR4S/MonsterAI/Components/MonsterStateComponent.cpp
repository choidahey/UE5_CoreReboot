#include "MonsterStateComponent.h"

UMonsterStateComponent::UMonsterStateComponent()
	: MyHeader(TEXT("MonsterStateComp"))
{
	PrimaryComponentTick.bCanEverTick = true;

	CurrentState = EMonsterState::Idle;
	PreviousState = EMonsterState::Idle;
	StateElapsedTime = 0.f;
}

void UMonsterStateComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register initial state
	StateDuration.Add(CurrentState, 0.f);

	// NOTICE :: Test Log
	UE_LOG(LogTemp, Log, TEXT("[%s] BeginPlay : CurrentState is %s."),
		*MyHeader,
		*UEnum::GetValueAsString(CurrentState)
	);
}

void UMonsterStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Accumulate time spent in current state
	StateElapsedTime += DeltaTime;
	StateDuration.FindOrAdd(CurrentState) += DeltaTime;
}

void UMonsterStateComponent::SetState(EMonsterState NewState)
{
	if (NewState == CurrentState)
	{
		return;
	}

	PreviousState = CurrentState;
	CurrentState = NewState;
	StateElapsedTime = 0.f;

	// Broadcast delegate when state changes
	OnStateChanged.Broadcast(PreviousState, CurrentState);
}

float UMonsterStateComponent::GetStateDuration(EMonsterState State) const
{
	if (const float* Found = StateDuration.Find(State))
	{
		return *Found;
	}

	return 0.f;
}

void UMonsterStateComponent::ForceInterrupt()
{
	// Force reset to Idle state
	SetState(EMonsterState::Idle);
}

void UMonsterStateComponent::SetPhase(EBossPhase NewPhase)
{
	if (CurrentPhase == NewPhase) return;

	CurrentPhase = NewPhase;
	OnPhaseChanged.Broadcast(NewPhase);
}
