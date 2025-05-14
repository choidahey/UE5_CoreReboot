#include "BaseMonster.h"
#include "Controller/BaseMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/MonsterAttributeComponent.h"
// #include "Components/MonsterSkillComponent.h"
#include "Components/MonsterStateComponent.h"
#include "Components/MonsterPerceptionComponent.h"
#include "Components/MonsterAnimComponent.h"

ABaseMonster::ABaseMonster()
	: MyHeader(TEXT("BaseMonster"))
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize Components
	AttributeComp = CreateDefaultSubobject<UMonsterAttributeComponent>(TEXT("AttributeComp"));
	//SkillComp = CreateDefaultSubobject<UMonsterSkillComponent>(TEXT("SkillComp"));
	StateComp = CreateDefaultSubobject<UMonsterStateComponent>(TEXT("StateComp"));
	PerceptionComp = CreateDefaultSubobject<UMonsterPerceptionComponent>(TEXT("PerceptionComp"));
	AnimComp = CreateDefaultSubobject<UMonsterAnimComponent>(TEXT("AnimComp"));

	AIControllerClass = ABaseMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABaseMonster::BeginPlay()
{
	Super::BeginPlay();

	// Bind OnDeath delegate
	if (AttributeComp)
	{
		AttributeComp->InitializeMonsterAttribute(MonsterID);
		AttributeComp->OnDeath.AddDynamic(this, &ABaseMonster::HandleDeath);
	}

	if (StateComp)
	{
		StateComp->OnStateChanged.AddDynamic(this, &ABaseMonster::OnMonsterStateChanged);
	}

	if (PerceptionComp)
	{
		PerceptionComp->OnPlayerDetected.AddDynamic(this, &ABaseMonster::OnTargetDetected);
		PerceptionComp->OnPlayerLost.AddDynamic(this, &ABaseMonster::OnTargetLost);
	}

	if (AnimComp)
	{
		AnimComp->Initialize(GetMesh());
	}
}

void ABaseMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (StateComp->IsInState(EMonsterState::Idle))
	{
		float TotalIdleTime = StateComp->GetStateDuration(EMonsterState::Idle);
		if (TotalIdleTime > 10.f)
		{
			//AnimComp->PlayIdleBoredAnimation();
		}
	}
}

void ABaseMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!AttributeComp || IsDead())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ABaseMonster] TakeDamage - AttributeComponent is null or Monster is already dead!"));
		return 0.0f;
	}

	// Apply damage via AttributeComponent
	AttributeComp->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void ABaseMonster::TryAttack()
{
	/*if (!SkillComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ABaseMonster] TryAttack - SkillComponent is null!"));
		return;
	}*/
	
	if (!CanAttack())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ABaseMonster] TryAttack - Monster cannot attack!"));
		return;
	}

	// Select an available skill and Execute
	//int32 SelectedSkillIndex = SkillComp->SelectAvailableSkill();
	//if (SelectedSkillIndex >= 0)
	//{
	//	PerfromAttack(SelectedSkillIndex);
	//}
}

void ABaseMonster::PerfromAttack(int32 AttackIndex)
{
	//if (!StateComp || !SkillComp || IsDead())
	//	return false;

	//// Check current state and Skill cooldown
	//return StateComp->GetCurrentState() != EMonsterState::Dead
	//	&& SkillComp->HasAvailableSkill();
}

bool ABaseMonster::CanAttack() const
{
	/*if (!StateComp || !SkillComp || IsDead())
	{
		return false;
	}*/

	return StateComp->IsInState(EMonsterState::Idle) ||
		StateComp->IsInState(EMonsterState::Patrol) ||
		StateComp->IsInState(EMonsterState::Chase);
}

void ABaseMonster::Die()
{
	if (StateComp)
	{
		StateComp->SetState(EMonsterState::Dead);
	}

	// Update Blackboard
	if (ABaseMonsterAIController* AIC = Cast<ABaseMonsterAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsBool(TEXT("IsDead"), true);
		}
	}

	// Play Death Montage
	if (AnimComp)
	{
		AnimComp->PlayDeathMontage();
	}
}

bool ABaseMonster::IsDead() const
{
	return AttributeComp && AttributeComp->IsDead();
}

void ABaseMonster::SetTargetActor(AActor* NewTarget)
{
	TargetActor = NewTarget;

	// Sync with Blackboard
	if (ABaseMonsterAIController* AIC = Cast<ABaseMonsterAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsObject(TEXT("TargetActor"), TargetActor);
		}
	}
}

void ABaseMonster::HandleDeath()
{
	if (bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BaseMonster] HandleDeath already called"));
		return;
	}

	bIsDead = true;
	Die();
}

void ABaseMonster::OnMonsterStateChanged(EMonsterState Previous, EMonsterState Current)
{
	// NOTICE :: Test Log
	UE_LOG(LogTemp, Log, TEXT("[%s] OnMonsterStateChanged : State changed from %s to %s."),
		*MyHeader,
		*UEnum::GetValueAsString(Previous),
		*UEnum::GetValueAsString(Current)
	);
}

void ABaseMonster::OnTargetDetected(AActor* DetectedActor)
{
	if (!IsDead())
	{
		SetTargetActor(DetectedActor);
	}

	// NOTICE :: Test Log
	UE_LOG(LogTemp, Log, TEXT("[%s] Target Detected: %s"),
		*MyHeader,
		*DetectedActor->GetName()
	);
}

void ABaseMonster::OnTargetLost(AActor* LostActor)
{
	if (TargetActor == LostActor)
	{
		// NOTICE :: Test Log
		UE_LOG(LogTemp, Log, TEXT("[%s] Lost Target: %s"),
			*MyHeader, 
			*LostActor->GetName());

		SetTargetActor(nullptr);
	}
}
