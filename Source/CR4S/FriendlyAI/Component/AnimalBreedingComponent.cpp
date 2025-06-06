#include "AnimalBreedingComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "../BaseAnimal.h"

// UAnimalBreedingComponent::UAnimalBreedingComponent()
// {
//     PrimaryComponentTick.bCanEverTick = false;
// }
//
// void UAnimalBreedingComponent::BeginPlay()
// {
//     Super::BeginPlay();
//     OwnerAnimal = Cast<ABaseAnimal>(GetOwner());
// }
//
// void UAnimalBreedingComponent::StartGestation(AActor* Partner)
// {
//     if (!OwnerAnimal || !OwnerAnimal->bIsFemale)
//     {
//         return;
//     }
//     if (bIsPregnant)
//     {
//         return;
//     }
//
//     if (const FAnimalStatsRow* Stats = OwnerAnimal->GetStatsRowPtr())
//     {
//         GestationPeriod = Stats->GestationPeriod;
//         GrowthDuration  = Stats->GrowthTime;
//     }
//     
//     PartnerActor    = Partner;
//     bIsPregnant     = true;
//
//     GetWorld()->GetTimerManager().SetTimer(
//         GestationTimerHandle,
//         this,
//         &UAnimalBreedingComponent::DoBirth,
//         GestationPeriod,
//         false
//     );
//
//     Log(FString::Printf(TEXT("Pregnant : Wait %.1f"), GestationPeriod));
// }
//
// void UAnimalBreedingComponent::DoBirth()
// {
//     if (!OwnerAnimal) return;
//
//     UWorld* World = GetWorld();
//     FVector Loc   = OwnerAnimal->GetActorLocation();
//     FActorSpawnParameters Params;
//     Params.Owner = OwnerAnimal;
//
//     OffspringActor = World->SpawnActor<AActor>(
//         OwnerAnimal->GetClass(),
//         Loc,
//         OwnerAnimal->GetActorRotation(),
//         Params
//     );
//
//     if (OffspringActor)
//     {
//         OffspringActor->SetActorScale3D(FVector(OffspringInitialScale));
//         Log(TEXT("Breed"));
//         StartGrowth();
//     }
//     bIsPregnant = false;
// }
//
// void UAnimalBreedingComponent::StartGrowth()
// {
//     if (GrowthDuration <= 0.f || !OffspringActor)
//     {
//         if (OffspringActor)
//             OffspringActor->SetActorScale3D(FVector(OffspringAdultScale));
//         Log(TEXT("Growth complete"));
//         return;
//     }
//
//     ElapsedGrowthTime = 0.f;
//     float TickInterval = GrowthDuration / 20.f;
//     GetWorld()->GetTimerManager().SetTimer(
//         GrowthTimerHandle,
//         this,
//         &UAnimalBreedingComponent::UpdateGrowth,
//         TickInterval,
//         true
//     );
// }
//
// void UAnimalBreedingComponent::UpdateGrowth()
// {
//     if (!OffspringActor) return;
//
//     ElapsedGrowthTime += GetWorld()->GetTimerManager().GetTimerRate(GrowthTimerHandle);
//     float Alpha = FMath::Clamp(ElapsedGrowthTime / GrowthDuration, 0.f, 1.f);
//     float NewScale = FMath::Lerp(OffspringInitialScale, OffspringAdultScale, Alpha);
//     OffspringActor->SetActorScale3D(FVector(NewScale));
//
//     if (Alpha >= 1.f)
//     {
//         GetWorld()->GetTimerManager().ClearTimer(GrowthTimerHandle);
//         Log(TEXT("Growth complete"));
//     }
// }
//
// void UAnimalBreedingComponent::Log(const FString& Message) const
// {
//     if (OwnerAnimal)
//         UE_LOG(LogTemp, Log, TEXT("[%s] %s"), *OwnerAnimal->GetName(), *Message);
// }