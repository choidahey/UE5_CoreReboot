#include "BaseAnimal.h"
#include "AIController.h"
#include "Controller/AnimalAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimalStatsSubsystem.h"
#include "Components/SphereComponent.h"
#include "UI/AnimalInteractWidget.h"
#include "../Inventory/Components/BaseInventoryComponent.h"
#include "Component/AnimalRangedAttackComponent.h"
#include "Components/ArrowComponent.h"
#include "Component/GroundMovementComponent.h"
#include "AnimalFlying.h"
#include "AnimalGround.h"
#include "AnimalMonster.h"
#include "../Character/Characters/PlayerCharacter.h"
#include "NavigationInvokerComponent.h"
#include "NiagaraComponent.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Components/BaseStatusComponent.h"
#include "Controller/AnimalMonsterAIController.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Utility/CombatStatics.h"

ABaseAnimal::ABaseAnimal()
{
    PrimaryActorTick.bCanEverTick = true;
    
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    AttackRange = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
    AttackRange->SetupAttachment(RootComponent);
    AttackRange->SetSphereRadius(150.f);
    AttackRange->SetCollisionProfileName(TEXT("Trigger"));

    EnemyCollision = CreateDefaultSubobject<USphereComponent>(TEXT("EnemyCollision"));
    EnemyCollision->SetupAttachment(RootComponent);
    EnemyCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    EnemyCollision->SetCollisionObjectType(ECC_GameTraceChannel2);
    EnemyCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    RangedAttackComponent = CreateDefaultSubobject<UAnimalRangedAttackComponent>(TEXT("RangedAttackComponent"));
    
    MuzzleArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("MuzzleArrow"));
    MuzzleArrow->SetupAttachment(RootComponent);

    GroundComp = CreateDefaultSubobject<UGroundMovementComponent>(TEXT("GroundMovementComponent"));

    NavInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvokerComponent"));
    NavInvokerComponent->SetGenerationRadii(2000.0f, 2500.0f);

    InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
    InteractableComponent->SetActive(false);

    StunEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("StunEffectComponent"));
    StunEffectComponent->SetupAttachment(RootComponent);
    StunEffectComponent->bAutoActivate = false;

    HitEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HitEffectComponent"));
    HitEffectComponent->SetupAttachment(RootComponent);
    HitEffectComponent->bAutoActivate = false;
}

void ABaseAnimal::BeginPlay()
{
    Super::BeginPlay();
    
    if (InteractableComponent)
    {
        InteractableComponent->OnTryInteract.AddUniqueDynamic(this, &ABaseAnimal::OnInteract);
    }
    
    if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
    {
        C->SetAnimalState(EAnimalState::Patrol);
    }

    if (APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
    {
        if (UBaseStatusComponent* StatusComp = Player->FindComponentByClass<UBaseStatusComponent>())
        {
            StatusComp->OnDeathState.AddLambda([this]()
            {
                bPlayerDead = true;

                if (Cast<APlayerCharacter>(CurrentTarget) || Cast<AModularRobot>(CurrentTarget))
                {
                    if (AAnimalAIController* AIController = Cast<AAnimalAIController>(GetController()))
                    {
                        AIController->ClearTargetActor();
                        AIController->SetAnimalState(EAnimalState::Patrol);
                    }
                }
            });
        }
    }
    
    LoadStats();
    
    bUseControllerRotationYaw = false;
}

void ABaseAnimal::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    DrawDebugVisuals();
}

void ABaseAnimal::LoadStats()
{
    if (auto* Subsys = GetGameInstance()->GetSubsystem<UAnimalStatsSubsystem>())
    {
        const FAnimalStatsRow* Row = Subsys->GetStatsRow(RowName);
        if (Row)
        {
            StatsRow = Row;
            if (!SoundData)
            {
                UE_LOG(LogTemp, Warning, TEXT("[%s] SoundData is not assigned"), *GetName());
            }
            CurrentStats = *Row;
            CurrentStats.RunSpeed = Row->RunSpeed;
            bStatsReady = true;
            CurrentHealth = Row->MaxHealth;
            JumpPower = Row->JumpPower;
            
            //CachedAttackInterval = Row->AttackInterval;

            bCanMelee  = (MeleeAttackMontage != nullptr);
            bCanCharge = (ChargeMontage != nullptr);
            bCanRanged = (RangedMontage != nullptr);

            const UEnum* EnumPtr = StaticEnum<EAnimalBehavior>();
            if (EnumPtr)
            {
                BehaviorTypeEnum = static_cast<EAnimalBehavior>(
                    EnumPtr->GetValueByName(FName(*Row->BehaviorType))
                );
            }

            if (BehaviorTypeEnum == EAnimalBehavior::Monster || BehaviorTypeEnum == EAnimalBehavior::Aggressive)
            {
                EnemyCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Overlap);
            }
            
            if (GetCharacterMovement())
            {
                GetCharacterMovement()->MaxWalkSpeed = CurrentStats.RunSpeed;
                GetCharacterMovement()->MaxAcceleration = 2048.f;
                GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
                GetCharacterMovement()->GroundFriction = 8.f;
            }

            if (AAIController* AIController = Cast<AAIController>(GetController()))
            {
                if (AAnimalAIController* AnimalAI = Cast<AAnimalAIController>(AIController))
                {
                    AnimalAI->ApplyPerceptionStats(CurrentStats);

                    UBehaviorTree* BTAsset = nullptr;
                    if (Cast<AAnimalGround>(this))
                    {
                        BTAsset = AnimalAI->GroundBehaviorTree;
                    }
                    else if (Cast<AAnimalFlying>(this))
                    {
                        BTAsset = AnimalAI->FlyingBehaviorTree;
                    }
                    // else if (Cast<AMonster>(this))
                    // {
                    //     BTAsset = AnimalAI->MonsterBehaviorTree;
                    // }

                    if (BTAsset)
                    {
                        AnimalAI->RunBehaviorTree(BTAsset);
                        AnimalAI->SetAnimalState(EAnimalState::Patrol);
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

#pragma region Debug
void ABaseAnimal::DrawDebugVisuals()
{
    if (bDrawSightDebug)
    {
        FVector EyesLoc;
        FRotator EyesRot;
        GetActorEyesViewPoint(EyesLoc, EyesRot);
        EyesRot.Pitch = 0.f;

        if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
        {
            if (UAISenseConfig_Sight* Sight = C->GetSightConfig())
            {
                float FOV = Sight->PeripheralVisionAngleDegrees;
                float HalfFOVRadians = FMath::DegreesToRadians(FOV * 0.5f);
                float SightRadius = Sight->SightRadius;
                float LoseRadius = Sight->LoseSightRadius;
                
                DrawDebugCone(
                    GetWorld(),
                    EyesLoc,
                    EyesRot.Vector(),
                    SightRadius,
                    HalfFOVRadians,
                    HalfFOVRadians,
                    12,
                    FColor::Green,
                    false,
                    -1.f,
                    0,
                    1.f
                );
                
                DrawDebugSphere(
                    GetWorld(),
                    EyesLoc,
                    SightRadius,
                    32,
                    FColor::Cyan,
                    false,
                    -1.f,
                    0,
                    1.f
                );
                
                DrawDebugSphere(
                    GetWorld(),
                    EyesLoc,
                    LoseRadius,
                    32,
                    FColor::Yellow,
                    false,
                    -1.f,
                    0,
                    1.f
                );
            }
        }
    }

    if (bDrawAttackRangeDebug)
    {
        DrawDebugSphere(
            GetWorld(),
            GetActorLocation(),
            MeleeRange,
            16,
            FColor::Red,
            false,
            -1.f,
            0,
            1.f
        );
        DrawDebugSphere(
            GetWorld(),
            GetActorLocation(),
            DashRange,
            16,
            FColor::Blue,
            false,
            -1.f,
            0,
            1.f
        );
        DrawDebugSphere(
            GetWorld(),
            GetActorLocation(),
            RangedRange,
            16,
            FColor::Green,
            false,
            -1.f,
            0,
            1.f
        );
    }
}

#pragma endregion

void ABaseAnimal::TakeStun_Implementation(const float StunAmount)
{
    ApplyStun(StunAmount);
}

void ABaseAnimal::ApplyStun(float Amount)
{
    if (!bStatsReady || !StatsRow || bIsStunned) return;

    StunValue += Amount;

    if (StunValue >= CurrentStats.StunThreshold)
    {
        if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
        {
            C->OnStunned();

            if (UAIPerceptionComponent* PerceptionComp = C->GetAIPerceptionComponent())
            {
                PerceptionComp->SetSenseEnabled(UAISense_Sight::StaticClass(), false);
                PerceptionComp->SetSenseEnabled(UAISense_Hearing::StaticClass(), false);
            }
        }
       
        bIsStunned = true;
        
        if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
        {
            MovementComp->DisableMovement();
        }
        
        if (USkeletalMeshComponent* MeshComp = GetMesh())
        {
            if (MeshComp->DoesSocketExist(TEXT("Neck")))
            {
                FName BoneName = MeshComp->GetSocketBoneName(TEXT("Neck"));
                MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
                MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
                MeshComp->SetAllBodiesBelowSimulatePhysics(BoneName, true, true);
            }
        }
        
        if (StunEffectComponent)
        {
            StunEffectComponent->ActivateSystem(true);
        }

        GetWorldTimerManager().SetTimer(StunRecoverTimer, this, &ABaseAnimal::RecoverFromStun, StatsRow->StunDuration, false);
    }
}

void ABaseAnimal::RecoverFromStun()
{
    if (CurrentState == EAnimalState::Dead) return;
    
    bIsStunned = false;
    StunValue = 0.f;
    
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->SetMovementMode(MOVE_Walking);
    }

    if (StunEffectComponent)
    {
        StunEffectComponent->DeactivateSystem();
    }

    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        FName BoneName = MeshComp->GetSocketBoneName(TEXT("TS"));
        MeshComp->SetAllBodiesBelowSimulatePhysics(BoneName, false, true);
        MeshComp->SetCollisionProfileName(TEXT("CharacterMesh"));
        MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
    {
        if (UAIPerceptionComponent* PerceptionComp = C->GetAIPerceptionComponent())
        {
            PerceptionComp->SetSenseEnabled(UAISense_Sight::StaticClass(), true);
            PerceptionComp->SetSenseEnabled(UAISense_Hearing::StaticClass(), true);
        }
        C->OnRecoveredFromStun();
    }
}

void ABaseAnimal::Die()
{
    if (CurrentState != EAnimalState::Dead) return;

    GetWorldTimerManager().ClearTimer(StunRecoverTimer);
    
    if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
    {
        C->OnDied();
    }
    else if (AAnimalMonsterAIController* MC = Cast<AAnimalMonsterAIController>(GetController()))
    {
        MC->OnDied();
    }

    if (StunEffectComponent)
    {
        StunEffectComponent->DeactivateSystem();
    }

    if (IsValid(ActiveInteractWidget))
    {
        ActiveInteractWidget->RemoveFromParent();
        ActiveInteractWidget = nullptr;
    }
    OnDied.Broadcast(this);

    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        if (UAIPerceptionComponent* Perception = AIController->FindComponentByClass<UAIPerceptionComponent>())
        {
            Perception->SetSenseEnabled(UAISense_Sight::StaticClass(), false);
        }
        if (UBrainComponent* Brain = AIController->BrainComponent)
        {
            Brain->StopLogic("Dead");  
        }
        AIController->StopMovement();
        AIController->UnPossess(); 
    }
        
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

    GetMesh()->SetSimulatePhysics(true);
    GetCharacterMovement()->Deactivate();

    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
    SetLifeSpan(30.f);

    ElapsedFadeTime = 0.f;
    GetWorldTimerManager().SetTimer(FadeDelayTimerHandle, this, &ABaseAnimal::StartFadeOut, 28.f, false);
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
    if (ActualDamage <= 0.f)
    {
        return 0.f;
    }
    
    ShowHitEffect(DamageCauser);

    CurrentHealth -= ActualDamage;
    if (CurrentHealth <= 0.f)
    {
        CurrentHealth = 0.f;
        SetAnimalState(EAnimalState::Dead);
        Die();
    }

    if (AAnimalAIController* C = Cast<AAnimalAIController>(GetController()))
    {
        C->SetTargetByDamage(DamageCauser);
    }

    return ActualDamage;
}

void ABaseAnimal::ShowHitEffect(AActor* DamageCauser)
{
    if (!HitEffectComponent || HitEffectSystems.Num() == 0 || !DamageCauser)
        return;
    
    int32 RandomIndex = FMath::RandRange(0, HitEffectSystems.Num() - 1);
    UNiagaraSystem* SelectedEffect = HitEffectSystems[RandomIndex];
    
    if (!SelectedEffect)
        return;
    
    FVector StartLocation = DamageCauser->GetActorLocation();
    FVector EndLocation = GetActorLocation();
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(DamageCauser);
    
    if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Pawn, QueryParams))
    {
        HitEffectComponent->SetWorldLocation(HitResult.Location);
        HitEffectComponent->SetAsset(SelectedEffect);
        HitEffectComponent->ActivateSystem(true);
    
        if (SoundData && CurrentState != EAnimalState::Dead)
        {
            PlayAnimalSound(SoundData->HitSounds, HitResult.Location, EConcurrencyType::Impact);
        }
    }
}

// void ABaseAnimal::SetbIsTamed(bool bNewValue)
// {
//     if (bIsTamed == bNewValue) return;
//
//     bIsTamed = bNewValue;
//
//     if (AAnimalAIController* AIController = Cast<AAnimalAIController>(GetController()))
//     {
//         if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
//         {
//             BB->SetValueAsBool(TEXT("IsTamed"), bNewValue);
//         }
//     }
// }

void ABaseAnimal::OnInteract(AActor* Interactor)
{
    if (CurrentState != EAnimalState::Dead) return;

    APlayerCharacter* Player = Cast<APlayerCharacter>(Interactor);
    if (!IsValid(Player)) return;

    UBaseInventoryComponent* Inventory = Player->FindComponentByClass<UBaseInventoryComponent>();
    if (!IsValid(Inventory)) return;

    if (!bStatsReady || !StatsRow || CurrentStats.DropItemRowNames.Num() == 0 || CurrentStats.TotalDropItemCount <= 0)
    {
        Destroy();
        return;
    }

    TMap<FName, int32> DroppedItems;

    for (int32 i = 0; i < CurrentStats.TotalDropItemCount; ++i)
    {
        int32 Index = FMath::RandRange(0, CurrentStats.DropItemRowNames.Num() - 1);
        FName SelectedItem = CurrentStats.DropItemRowNames[Index];
        DroppedItems.FindOrAdd(SelectedItem)++;
    }

    if (DroppedItems.Num() > 0)
    {
        Inventory->AddItems(DroppedItems);
    }

    if (SoundData)
    {
        PlayAnimalSound(SoundData->InteractionSounds, GetActorLocation(), EConcurrencyType::Default);
    }
    
    StartFadeOut();
    SetLifeSpan(2.0f);
}

void ABaseAnimal::GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const
{
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        Location = MeshComp->GetSocketLocation(TEXT("head"));
        FRotator RawRot = MeshComp->GetSocketRotation(TEXT("head"));
        Rotation = FRotator(0.f, RawRot.Yaw, 0.f);
    }
    else
    {
        Super::GetActorEyesViewPoint(Location, Rotation);
    }
}

#pragma region Attack

float ABaseAnimal::PlayAttackMontage()
{
    if (bIsAttacking || !MeleeAttackMontage) return 0.0f;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        float MontageDuration = AnimInstance->Montage_Play(MeleeAttackMontage);
        bIsAttacking = true;
        bIsMeleeOnCooldown = true;

        if (UCharacterMovementComponent* Move = GetCharacterMovement())
        {
            Move->StopMovementImmediately();
        }
        
        GetWorldTimerManager().SetTimer(
            AttackTimerHandle,
            this,
            &ABaseAnimal::ResetAttackFlag,
            MontageDuration,
            false
        );
        
        GetWorldTimerManager().SetTimer(
            MeleeAttackTimerHandle,
            this,
            &ABaseAnimal::ResetMeleeAttack,
            MeleeAttackCooldown,
            false
        );
        return MontageDuration;
    }
    return 0.0f;
}

float ABaseAnimal::PlayChargeAttackMontage()
{
    if (bIsAttacking || !ChargeMontage) return 0.0f;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        if (UCharacterMovementComponent* Move = GetCharacterMovement())
        {
            Move->StopMovementImmediately();
        }
        
        float MontageDuration = AnimInstance->Montage_Play(ChargeMontage);
        bIsAttacking = true;
        bIsChargeOnCooldown = true;
        
        GetWorldTimerManager().SetTimer(
            AttackTimerHandle,
            this,
            &ABaseAnimal::ResetAttackFlag,
            MontageDuration,
            false
        );
        
        GetWorldTimerManager().SetTimer(
            ChargeAttackTimerHandle,
            this,
            &ABaseAnimal::ResetChargeAttack,
            ChargeAttackCooldown,
            false
        );
        return MontageDuration;
    }
    return 0.0f;
}

float ABaseAnimal::PlayRangedAttackMontage()
{
    if (bIsAttacking || !RangedMontage) return 0.0f;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        if (UCharacterMovementComponent* Move = GetCharacterMovement())
        {
            Move->StopMovementImmediately();
        }
        float MontageDuration = AnimInstance->Montage_Play(RangedMontage);
        bIsAttacking = true;
        bIsRangedOnCooldown = true;
        
        GetWorldTimerManager().SetTimer(
            AttackTimerHandle,
            this,
            &ABaseAnimal::ResetAttackFlag,
            MontageDuration,
            false
        );
        
        GetWorldTimerManager().SetTimer(
            RangedAttackTimerHandle,
            this,
            &ABaseAnimal::ResetRangedAttack,
            RangedAttackCooldown,
            false
        );
        return MontageDuration;
    }

    return 0.0f;
}

void ABaseAnimal::PerformMeleeAttack()
{
    if (!CurrentTarget || !bCanMelee || bIsMeleeOnCooldown) return;

    const float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
    if (Distance > MeleeRange) return;
        
    GetWorldTimerManager().SetTimer(
        MeleeAttackTimerHandle,
        this,
        &ABaseAnimal::ResetMeleeCooldown,
        MeleeAttackCooldown,
        false
    );
}

void ABaseAnimal::PerformChargeAttack()
{
    if (!bCanCharge || bIsChargeOnCooldown) return;
    
    GetWorldTimerManager().SetTimer(
        ChargeAttackTimerHandle,
        this,
        &ABaseAnimal::ResetChargeCooldown,
        ChargeAttackCooldown,
        false
    );
}

void ABaseAnimal::PerformRangedAttack()
{
    // if (!bCanRanged || bIsRangedOnCooldown)
    // {
    //     UE_LOG(LogTemp, Error, TEXT("bCanRanged: %s, bIsRangedOnCooldown: %s"),
    // bCanRanged ? TEXT("true") : TEXT("false"),
    // bIsRangedOnCooldown ? TEXT("true") : TEXT("false"));
    //
    //     return;
    // }
    if (RangedAttackComponent)
    {
        RangedAttackComponent->FireProjectile();
    }
    
    GetWorldTimerManager().SetTimer(
        RangedAttackTimerHandle,
        this,
        &ABaseAnimal::ResetRangedCooldown,
        RangedAttackCooldown,
        false
    );
}
#pragma endregion

#pragma region Pade Out Effect
void ABaseAnimal::StartFadeOut()
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

    FTimerDelegate FadeDelegate = FTimerDelegate::CreateUObject(this, &ABaseAnimal::UpdateFade);
    GetWorldTimerManager().SetTimer(FadeTimerHandle, FadeDelegate, 0.02f, true);

    MeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
}

void ABaseAnimal::UpdateFade()
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
#pragma endregion

#pragma region SFX
void ABaseAnimal::PlayAnimalSound(const TArray<USoundBase*>& SoundArray, const FVector& Location, const EConcurrencyType SoundType, const float Pitch, const float StartTime) const
{
    if (SoundArray.Num() == 0)
        return;
       
    int32 RandomIndex = FMath::RandRange(0, SoundArray.Num() - 1);
    USoundBase* SelectedSound = SoundArray[RandomIndex];
   
    if (SelectedSound)
    {
        const UGameInstance* GameInstance = GetGameInstance();
        if (IsValid(GameInstance))
        {
            UAudioManager* AudioManager = GameInstance->GetSubsystem<UAudioManager>();
            if (IsValid(AudioManager))
            {
                AudioManager->PlaySFX(SelectedSound, Location, SoundType, Pitch, StartTime);
            }
        }
    }
}
#pragma endregion