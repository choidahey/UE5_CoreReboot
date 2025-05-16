#include "BaseAnimal.h"
#include "AIController.h"
#include "Controller/AnimalAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"  
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimalStatsSubsystem.h"

ABaseAnimal::ABaseAnimal()
{
    PrimaryActorTick.bCanEverTick = false;
    StunValue     = 0.f;
    CurrentTarget = nullptr;
}

void ABaseAnimal::BeginPlay()
{
    Super::BeginPlay();
    LoadStats();
}

void ABaseAnimal::LoadStats()
{
    if (auto* Subsys = GetGameInstance()->GetSubsystem<UAnimalStatsSubsystem>())
    {
        Subsys->GetStatsRowAsync(RowName, [this](const FAnimalStatsRow* Row)
        {
            if (Row)
            {
                StatsRow = Row;
                CurrentStats = *Row;
                bStatsReady = true;

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
                    }
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("Animal Stats Load Fail"));
            }
        });
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
    FVector Start = GetActorLocation() + GetActorForwardVector() * 100.f + FVector(0.f, 0.f, 50.f);
    FVector End = Start;

    float Radius = 100.f;
    float HalfHeight = 100.f;
    FQuat Rotation = FQuat::Identity;
    
    TArray<FHitResult> HitResults;
    FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(Radius, HalfHeight);

    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        Rotation,
        ECC_Pawn,
        CapsuleShape
    );

    FColor CapsuleColor = bHit ? FColor::Red : FColor::Green;
    DrawDebugCapsule(
        GetWorld(),
        Start,
        HalfHeight,
        Radius,
        Rotation,
        CapsuleColor,
        false,
        1.f
    );

    if (!bHit) return;

    for (const FHitResult& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (!HitActor || HitActor == this) continue;

        float Damage = CurrentStats.AttackDamage;

        UGameplayStatics::ApplyDamage(
            HitActor,
            Damage,
            GetController(),
            this,
            nullptr
        );
        
        break;
    }
}

void ABaseAnimal::ApplyStun(float Amount)
{
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
        Die();
    }
}

void ABaseAnimal::Die()
{
    if (CurrentState == EAnimalState::Dead) return;
    
    SetAnimalState(EAnimalState::Dead);
    OnDied.Broadcast();

    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        if (UAIPerceptionComponent* Perception = AIController->FindComponentByClass<UAIPerceptionComponent>())
        {
            Perception->ForgetActor(this);
        }
    }

    UE_LOG(LogTemp, Log,
        TEXT("[%s] Die"), *GetClass()->GetName());

    GetMesh()->SetSimulatePhysics(true);
    GetCharacterMovement()->Deactivate();
    SetLifeSpan(10.f);
}

void ABaseAnimal::SetAnimalState(EAnimalState NewState)
{
    CurrentState = NewState;
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
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (ActualDamage <= 0.f) return 0.f;

    CurrentHealth -= ActualDamage;
    if (CurrentHealth <= 0.f)
    {
        Die();
    }
    return ActualDamage;
}