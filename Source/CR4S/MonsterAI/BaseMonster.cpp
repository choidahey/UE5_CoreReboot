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
#include "Game/System/AudioManager.h"
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

	if (StateComponent)
	{
		StateComponent->InitializeStunData(AttributeComponent->GetMonsterAttribute());
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
	AddAccumulatedDamage(DamageAmount);

	return DamageAmount;
}

void ABaseMonster::TakeStun_Implementation(const float StunAmount)
{
	if (StateComponent)
		StateComponent->AddStun(StunAmount);
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

	if (UAudioManager* AudioMgr = GetGameInstance()->GetSubsystem<UAudioManager>())
	{
		AudioMgr->StopBGM();
	}
	
	bIsDead = true;
	StateComponent->SetState(EMonsterState::Dead);
	OnDied.Broadcast(this);

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		MoveComp->DisableMovement();

	if (ABaseMonsterAIController* AIC = Cast<ABaseMonsterAIController>(GetController()))
	{
		AIC->StopMovement();
		
		if (UBehaviorTreeComponent* BTComp = AIC->FindComponentByClass<UBehaviorTreeComponent>())
			BTComp->StopTree(EBTStopMode::Safe);

		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
			BB->SetValueAsBool(FAIKeys::IsDead, true);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AnimComponent)
	{
		AnimComponent->PlayDeathMontage();
	}

	SetLifeSpan(2.f);
	StartFadeOut();
}

void ABaseMonster::StartFadeOut()
{    
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;
    
	for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
	{
		if (UMaterialInstanceDynamic* DynMat = MeshComp->CreateAndSetMaterialInstanceDynamic(i))
		{
			DynMat->SetScalarParameterValue(TEXT("Appearance"), 1.0f);
		}
	}

	FTimerDelegate FadeDelegate = FTimerDelegate::CreateUObject(this, &ABaseMonster::UpdateFade);
	GetWorldTimerManager().SetTimer(FadeTimerHandle, FadeDelegate, 0.02f, true);

	MeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
}

void ABaseMonster::UpdateFade()
{
	ElapsedFadeTime += 0.02f;
	float NewAppearance = FMath::Lerp(1.0f, 0.0f, ElapsedFadeTime / 2.0f);
    
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
		{
			if (UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(MeshComp->GetMaterial(i)))
			{
				DynMat->SetScalarParameterValue(TEXT("Appearance"), NewAppearance);
			}
		}
	}
    
	if (ElapsedFadeTime >= 2.0f)
	{
		GetWorldTimerManager().ClearTimer(FadeTimerHandle);
	}
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
