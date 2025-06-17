#include "MonsterStateComponent.h"

#include "CR4S.h"
#include "MonsterAnimComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Data/MonsterAttributeRow.h"

UMonsterStateComponent::UMonsterStateComponent()
	: MyHeader(TEXT("MonsterStateComp"))
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

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

	if (!bIsStunned && CurrentStun > 0.f)
	{
		RecoveryElapsedTime = FMath::Clamp(RecoveryElapsedTime + DeltaTime, 0.f, StunRecoveryRampUpTime);
		
		float RecoveryAlpha = (StunRecoveryRampUpTime > 0.f) ? (RecoveryElapsedTime / StunRecoveryRampUpTime) : 1.f;
		float CurrentRecoveryRate = FMath::Lerp(StunRecoveryMin, StunRecoveryMax, RecoveryAlpha);
		
		CurrentStun = FMath::Clamp(CurrentStun - CurrentRecoveryRate * DeltaTime, 0.f, MaxStun);
		CR4S_Log(LogMonster, Log, TEXT("[Stun] CurrentStun=%.2f"), CurrentStun);
	}
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


void UMonsterStateComponent::ApplyStun(float StunAmount)
{
	if (bIsStunned) return;
	
	CurrentStun = FMath::Clamp(CurrentStun + StunAmount, 0.f, MaxStun);
	CR4S_Log(LogMonster, Log, TEXT("[Stun] CurrentStun=%.2f / MaxStun=%.2f"), CurrentStun, MaxStun);

	if (CurrentStun >= MaxStun)
	{
		bIsStunned = true;
		CurrentStun = MaxStun;
            	
		SetState(EMonsterState::Stunned);
            	
		if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
			if (UCharacterMovementComponent* Movement = Owner->GetCharacterMovement())
				Movement->DisableMovement();
            	
		if (ABaseMonster* Monster = Cast<ABaseMonster>(GetOwner()))
			if (Monster->AnimComponent)
				Monster->AnimComponent->PlayStunnedMontage();
		
		GetWorld()->GetTimerManager().ClearTimer(StunRecoveryTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			StunRecoveryTimerHandle,
			this,
			&UMonsterStateComponent::RemoveStunDebuff,
			StunChargeStartDelay,
			false
		);
	}
}

void UMonsterStateComponent::RemoveStunDebuff()
{
	if (!bIsStunned) return;

	bIsStunned = false;
	CurrentStun = 0.f;
	RecoveryElapsedTime = 0.f;

	SetState(EMonsterState::Idle);
	GetWorld()->GetTimerManager().ClearTimer(StunRecoveryTimerHandle);
	
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
		if (UCharacterMovementComponent* Movement = Owner->GetCharacterMovement())
			Movement->SetMovementMode(EMovementMode::MOVE_Walking);

	CR4S_Log(LogMonster, Log, TEXT("Stunned! Remove stun"));
}

void UMonsterStateComponent::InitializeStunData(const FMonsterAttributeRow& Data)
{
	MaxStun                = Data.MaxStun;
	StunDamageMultiplier   = Data.StunDamageMultiplier;
	StunChargeStartDelay   = Data.StunChargeStartDelay;
	StunRecoveryMin        = Data.StunRecoveryMin;
	StunRecoveryMax        = Data.StunRecoveryMax;
	StunRecoveryRampUpTime = Data.StunRecoveryRampUpTime;

	CR4S_Log(LogMonster, Log, TEXT("[%s] InitializeStunData: MaxStun=%.2f, StunDamageMultiplier=%.2f, StunChargeStartDelay=%.2f, StunRecoveryMin=%.2f, StunRecoveryMax=%.2f, StunRecoveryRampUpTime=%.2f"),
	   *MyHeader,
	   MaxStun,
	   StunDamageMultiplier,
	   StunChargeStartDelay,
	   StunRecoveryMin,
	   StunRecoveryMax,
	   StunRecoveryRampUpTime
   );
}
