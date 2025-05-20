#include "BaseMonster.h"
#include "Controller/BaseMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/MonsterAttributeComponent.h"
#include "Components/MonsterSkillComponent.h"
#include "Components/MonsterStateComponent.h"
#include "Components/MonsterAnimComponent.h"
#include "Data/MonsterAIKeyNames.h" 

ABaseMonster::ABaseMonster()
	: MyHeader(TEXT("BaseMonster"))
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize Components
	AttributeComponent = CreateDefaultSubobject<UMonsterAttributeComponent>(TEXT("AttributeComp"));
	SkillComponent = CreateDefaultSubobject<UMonsterSkillComponent>(TEXT("SkillComp"));
	StateComponent = CreateDefaultSubobject<UMonsterStateComponent>(TEXT("StateComp"));
	AnimComponent = CreateDefaultSubobject<UMonsterAnimComponent>(TEXT("AnimComp"));

	AIControllerClass = ABaseMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABaseMonster::BeginPlay()
{
	Super::BeginPlay();

	// Bind OnDeath delegate
	if (AttributeComponent)
	{
		AttributeComponent->InitializeMonsterAttribute(MonsterID);
		AttributeComponent->OnDeath.AddDynamic(this, &ABaseMonster::HandleDeath);
	}

	if (SkillComponent)
	{
		SkillComponent->InitializeMonsterSkills(MonsterID);
	}

	if (StateComponent)
	{
		StateComponent->OnStateChanged.AddDynamic(this, &ABaseMonster::OnMonsterStateChanged);
	}

	if (AnimComponent)
	{
		AnimComponent->Initialize(GetMesh());
	}
}

void ABaseMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead())
	{
		return;
	}

	if (StateComponent->IsInState(EMonsterState::Idle))
	{
		float TotalIdleTime = StateComponent->GetStateDuration(EMonsterState::Idle);
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
	if (!AttributeComponent || IsDead())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ABaseMonster] TakeDamage - AttributeComponent is null or Monster is already dead!"));
		return 0.0f;
	}

	// Apply damage via AttributeComponent
	AttributeComponent->ApplyDamage(DamageAmount);

	return DamageAmount;
}

void ABaseMonster::PlayPreMontage(int32 SkillIndex)
{
	if (SkillComponent)
	{
		SkillComponent->PlayPreMontage(SkillIndex);
	}
}

void ABaseMonster::UseSkill(int32 SkillIndex)
{
	if (SkillComponent)
	{
		SkillComponent->UseSkill(SkillIndex);
	}
}

void ABaseMonster::Die()
{
	if (StateComponent)
	{
		StateComponent->SetState(EMonsterState::Dead);
	}

	// Update Blackboard
	if (ABaseMonsterAIController* AIC = Cast<ABaseMonsterAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsBool(FAIKeys::IsDead, true);
		}
	}

	// Play Death Montage
	if (AnimComponent)
	{
		AnimComponent->PlayDeathMontage();
	}
}

bool ABaseMonster::IsDead() const
{
	return AttributeComponent && AttributeComponent->IsDead();
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
