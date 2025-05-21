#include "BaseMonster.h"
#include "Controller/BaseMonsterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/MonsterAttributeComponent.h"
#include "Components/MonsterSkillComponent.h"
#include "Components/MonsterStateComponent.h"
#include "Components/MonsterAnimComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/MonsterAIKeyNames.h" 
#include "GameFramework/CharacterMovementComponent.h"

ABaseMonster::ABaseMonster()
	: MyHeader(TEXT("BaseMonster"))
{
	PrimaryActorTick.bCanEverTick = false;

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

	if (ABaseMonsterAIController* AIC = Cast<ABaseMonsterAIController>(GetController()))
	{
		AIC->SetupPerceptionFromMonster(this);
	}
}

void ABaseMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void ABaseMonster::UseSkill(int32 SkillIndex)
{
	if (SkillComponent)
	{
		SkillComponent->UseSkill(SkillIndex);
	}
}

int32 ABaseMonster::SelectSkillIndex()
{
	return 0;
}

void ABaseMonster::Die()
{
	
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
	StateComponent->SetState(EMonsterState::Dead);

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		MoveComp->DisableMovement();

	if (ABaseMonsterAIController* AIC = Cast<ABaseMonsterAIController>(GetController()))
	{
		AIC->StopMovement();

		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
			BB->SetValueAsBool(FAIKeys::IsDead, true);
	}

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AnimComponent)
	{
		AnimComponent->PlayDeathMontage();
	}

	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		if (UAnimMontage* DeathMontage = AnimComponent->GetDeathMontage())
		{
			FOnMontageEnded EndDel;
			EndDel.BindUObject(this, &ABaseMonster::OnDeathMontageEnded);
			AnimInst->Montage_SetEndDelegate(EndDel, DeathMontage);
		}
	}
}

void ABaseMonster::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
		return;

	SetLifeSpan(4.f);
}


void ABaseMonster::OnMonsterStateChanged(EMonsterState Previous, EMonsterState Current)
{
	if (auto* AIC = Cast<ABaseMonsterAIController>(GetController()))
	{
		if (auto* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsInt(FAIKeys::CurrentState, static_cast<int32>(Current));
		}
	}

	// NOTICE :: Test Log
	UE_LOG(LogTemp, Log, TEXT("[%s] OnMonsterStateChanged : State changed from %s to %s."),
		*MyHeader,
		*UEnum::GetValueAsString(Previous),
		*UEnum::GetValueAsString(Current)
	);
}
