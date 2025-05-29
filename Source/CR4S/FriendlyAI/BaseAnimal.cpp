#include "BaseAnimal.h"
#include "AIController.h"
#include "Controller/AnimalAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/CapsuleComponent.h"
#include "../Gimmick/Components/InteractableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimalStatsSubsystem.h"
#include "Components/SphereComponent.h"
#include "UI/AnimalInteractWidget.h"
#include "../Character/Characters/PlayerCharacter.h"
#include "Inventory/Components/BaseInventoryComponent.h"

ABaseAnimal::ABaseAnimal()
{
    PrimaryActorTick.bCanEverTick = true;

    // Possess AI Automatically when Spawned
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    StunValue     = 0.f;
    CurrentTarget = nullptr;

    InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
    InteractableComponent->SetActive(false);

    AttackRange = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
    AttackRange->SetupAttachment(RootComponent);
    AttackRange->SetSphereRadius(150.f);
    AttackRange->SetCollisionProfileName(TEXT("Trigger"));
}

void ABaseAnimal::BeginPlay()
{
    Super::BeginPlay();
    if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
    {
        C->SetAnimalState(EAnimalState::Patrol);
    }
    LoadStats();
    if (InteractableComponent)
    {
        InteractableComponent->OnTryInteract.BindDynamic(this, &ABaseAnimal::OnInteract);
    }

    bUseControllerRotationYaw = false;

    UCharacterMovementComponent* Move = GetCharacterMovement();
    if (Move)
    {
        Move->bOrientRotationToMovement = true;
        Move->RotationRate = FRotator(0.f, 120.f, 0.f);
    }
}

void ABaseAnimal::LoadStats()
{
    if (auto* Subsys = GetGameInstance()->GetSubsystem<UAnimalStatsSubsystem>())
    {
        const FAnimalStatsRow* Row = Subsys->GetStatsRow(RowName);
        if (Row)
        {
            StatsRow = Row;
            CurrentStats = *Row;
            bStatsReady = true;
            CurrentHealth = Row->MaxHealth;
            CachedAttackInterval = Row->AttackInterval;

            const UEnum* EnumPtr = StaticEnum<EAnimalBehavior>();
            if (EnumPtr)
            {
                BehaviorTypeEnum = static_cast<EAnimalBehavior>(
                    EnumPtr->GetValueByName(FName(*Row->BehaviorType))
                );
            }

            if (GetCharacterMovement())
            {
                GetCharacterMovement()->MaxWalkSpeed = CurrentStats.WalkSpeed;
                GetCharacterMovement()->MaxAcceleration = 2048.f;
                GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
                GetCharacterMovement()->GroundFriction = 8.f;
            }

            if (AAIController* AIController = Cast<AAIController>(GetController()))
            {
                if (AAnimalAIController* AnimalAI = Cast<AAnimalAIController>(AIController))
                {
                    AnimalAI->ApplyPerceptionStats(CurrentStats);

                    if (UBehaviorTree* BTAsset = AnimalAI->GetBehaviorTreeAsset())
                    {
                        AnimalAI->RunBehaviorTree(BTAsset);
                        if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
                        {
                            C->SetAnimalState(EAnimalState::Patrol);
                        }
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Animal Stats Load Fail"));
            bStatsReady = false;
            return;
        }
    }
}

void ABaseAnimal::MoveToLocation(const FVector& Dest)
{   
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        FAIMoveRequest Req;
        Req.SetGoalLocation(Dest);
        Req.SetAcceptanceRadius(5.f);
        AIC->MoveTo(Req);
    }
}

void ABaseAnimal::PerformAttack()
{
    if (!AttackRange || !CurrentTarget) return;

    TArray<AActor*> OverlappedActors;
    AttackRange->GetOverlappingActors(OverlappedActors);

    if (!OverlappedActors.Contains(CurrentTarget)) return;

    if (!IsValid(CurrentTarget)) return;

    if (ABaseAnimal* HitAnimal = Cast<ABaseAnimal>(CurrentTarget))
    {
        if (HitAnimal->CurrentState == EAnimalState::Dead)
        {
            if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
            {
                C->OnTargetDied();
            }
            return;
        }
    }

    if (!AttackRange->IsOverlappingActor(CurrentTarget))
    {
        if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
        {
            C->OnTargetOutOfRange();
        }
        return;
    }

    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->StopMovementImmediately();
    }

    // TODO: player dead

    float Damage = CurrentStats.AttackDamage;
    UGameplayStatics::ApplyDamage(CurrentTarget, Damage, GetController(), this, nullptr);
}


void ABaseAnimal::ApplyStun(float Amount)
{
    if (!bStatsReady || !StatsRow) return;
    StunValue += Amount;
    UE_LOG(LogTemp, Log,
        TEXT("[%s] StunValue %.1f / %.1f"),
        *GetClass()->GetName(),
        StunValue,
        CurrentStats.StunThreshold
    );
    if (StunValue >= CurrentStats.StunThreshold)
    {
        UE_LOG(LogTemp, Log,
            TEXT("[%s] stun"), *GetClass()->GetName());
        
        if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
        {
            C->OnStunned();
        }
        bIsStunned = true;

        bUseControllerRotationYaw = false;
        GetCharacterMovement()->bOrientRotationToMovement = false;

        if (GetController())
        {
            GetController()->StopMovement();
        }
        GetCharacterMovement()->StopMovementImmediately();
        GetCharacterMovement()->DisableMovement();

        FName BoneName = GetMesh()->GetSocketBoneName(TEXT("TS"));
        GetMesh()->SetSimulatePhysics(false);
        GetMesh()->SetAllBodiesBelowSimulatePhysics(BoneName, true, true);
        GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

        GetWorldTimerManager().SetTimer(StunRecoverTimer, this, &ABaseAnimal::RecoverFromStun, StatsRow->StunDuration, false);
    }
}

void ABaseAnimal::RecoverFromStun()
{
    bIsStunned = false;

    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    GetMesh()->bBlendPhysics = true;
    GetMesh()->SetAllBodiesSimulatePhysics(false);
    GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
    {
        C->OnRecoveredFromStun();
    }

    if (IsValid(ActiveInteractWidget))
    {
        ActiveInteractWidget->RemoveFromParent();
        ActiveInteractWidget = nullptr;
    }
}

void ABaseAnimal::Die()
{
    if (CurrentState == EAnimalState::Dead) return;
    
    if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
    {
        C->OnDied();
    }

    if (IsValid(ActiveInteractWidget))
    {
        ActiveInteractWidget->RemoveFromParent();
        ActiveInteractWidget = nullptr;
    }
    
    OnDied.Broadcast(this); // One Param

    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        if (UAIPerceptionComponent* Perception = AIController->FindComponentByClass<UAIPerceptionComponent>())
        {
            Perception->ForgetActor(this);
        }
    }

    UE_LOG(LogTemp, Log,
        TEXT("[%s] Die"), *GetClass()->GetName());

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

    GetMesh()->SetSimulatePhysics(true);
    GetCharacterMovement()->Deactivate();

    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);    
    SetLifeSpan(30.f);
}

void ABaseAnimal::SetAnimalState(EAnimalState NewState)
{
    CurrentState = NewState;
    
    if (USkeletalMeshComponent* Skeletal = GetMesh())
    {
        const bool bEnableInteraction = (NewState == EAnimalState::Stun || NewState == EAnimalState::Dead);
        Skeletal->SetCollisionResponseToChannel(ECC_GameTraceChannel1, bEnableInteraction ? ECR_Block : ECR_Ignore);
    }
    
    AAIController* AIController = Cast<AAIController>(Controller);
    if (AIController)
    {
        UBlackboardComponent* BB = AIController->GetBlackboardComponent();
        if (BB)
        {
            BB->SetValueAsEnum(TEXT("AnimalState"), static_cast<uint8>(NewState));
        }
    }
}

void ABaseAnimal::ClearTarget()
{
    CurrentTarget = nullptr;
}

void ABaseAnimal::PlayAttackMontage()
{
    if (bIsAttacking || !AttackMontage) return;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        AnimInstance->Montage_Play(AttackMontage);
        bIsAttacking = true;
    }
}

float ABaseAnimal::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (ABaseAnimal* Damager = Cast<ABaseAnimal>(DamageCauser))
    {
        if (Damager->RowName == this->RowName)
        {
            return 0.f;
        }
    }

    float ActualDamage = DamageAmount;
    if (ActualDamage <= 0.f) return 0.f;

    CurrentHealth -= ActualDamage;
    if (CurrentHealth <= 0.f)
    {
        Die();
    }

    if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
    {
        C->SetTargetByDamage(DamageCauser);
    }

    return ActualDamage;
}

void ABaseAnimal::SetbIsTamed(bool bNewValue)
{
    if (bIsTamed == bNewValue) return;

    bIsTamed = bNewValue;

    if (AAnimalAIController* AIController = Cast<AAnimalAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
        {
            BB->SetValueAsBool(TEXT("IsTamed"), bNewValue);
        }
    }
}

void ABaseAnimal::OnInteract(AActor* Interactor)
{
    if (!IsValid(InteractWidgetClass)) return;

    UAnimalInteractWidget* InteractUI = CreateWidget<UAnimalInteractWidget>(GetWorld(), InteractWidgetClass);
    if (IsValid(InteractUI))
    {
        InteractUI->OwningAnimal = this;
        InteractUI->InitByAnimalState(bIsStunned);
        InteractUI->AddToViewport();
        ActiveInteractWidget = InteractUI;
    }
}

void ABaseAnimal::Capture()
{
    APlayerCharacter* Interactor = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!IsValid(Interactor)) return;

    UBaseInventoryComponent* Inventory = Interactor->FindComponentByClass<UBaseInventoryComponent>();
    if (!IsValid(Inventory)) return;

    const FAddItemResult Result = Inventory->AddItem(RowName, 1);
    if (Result.Success)
    {
        if (IsValid(ActiveInteractWidget))
        {
            ActiveInteractWidget->RemoveFromParent();
            ActiveInteractWidget = nullptr;
        }

        Destroy();
    }
}

void ABaseAnimal::Butcher()
{
    // TODO : AddItem
    Destroy();
}