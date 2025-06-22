#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "CR4S.h"
#include "NavigationInvokerComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Game/System/EnvironmentalModifierVolume.h"
#include "Game/System/EnvironmentManager.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterAggroComponent.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

ASeasonBossMonster::ASeasonBossMonster()
{
   NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
   NavInvoker->SetGenerationRadii(NavInvokerRadius, NavInvokerRemovalRadius);
   
   AggroComp = CreateDefaultSubobject<UMonsterAggroComponent>(TEXT("AggroComp"));
   AggroTargetKey.SelectedKeyName = FAIKeys::TargetActor;
}

void ASeasonBossMonster::BeginPlay()
{
   Super::BeginPlay();

   SpawnOpeningPattern();
}


float ASeasonBossMonster::TakeDamage(
   float DamageAmount,
   FDamageEvent const& DamageEvent,
   AController* EventInstigator,
   AActor* DamageCauser)
{
   float Actual = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

   if (AggroComp && DamageCauser)
   {
      AggroComp->AddDamageAggro(DamageCauser, Actual);
   }
   return Actual;
}

void ASeasonBossMonster::HandleDeath()
{
   Super::HandleDeath();

   if (SpawnedEnvVolume && SpawnedEnvVolume->IsValidLowLevel())
   {
      SpawnedEnvVolume->Destroy();
      SpawnedEnvVolume = nullptr;
   }

   if (SpawnedNiagaraComp && SpawnedNiagaraComp->IsValidLowLevel())
   {
      SpawnedNiagaraComp->DestroyComponent();
      SpawnedNiagaraComp = nullptr;
   }
}

void ASeasonBossMonster::SpawnOpeningPattern()
{
   UWorld* World = GetWorld();
   if (!World) return;
   
   if (AEnvironmentManager* EnvManager = Cast<AEnvironmentManager>(UGameplayStatics::GetActorOfClass(World, AEnvironmentManager::StaticClass())))
   {
      SpawnedEnvVolume = EnvManager->SpawnEnvModVol(
         GetActorLocation(),
         EnvVolRadius,
         EnvVolHeight,
         EnvTempDelta,
         EnvHumidDelta,
         EnvVolChangeSpeed,
         -1.f);
      
      if (IsValid(SpawnedEnvVolume))
      {
         SpawnedEnvVolume->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
         SpawnedEnvVolume->SetActorRelativeLocation(FVector::ZeroVector);
      }
   }

   FVector ActorLoc = GetActorLocation();
   const float TraceHeight = 2000.f;
   FHitResult Hit;
   FVector TraceStart = ActorLoc + FVector(0,0,TraceHeight);
   FVector TraceEnd = ActorLoc - FVector(0,0,TraceHeight);
   FCollisionQueryParams Params;
   Params.AddIgnoredActor(this);

   float GroundOffsetZ = 0.f;
   if ( World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, Params) )
      GroundOffsetZ = Hit.Location.Z - ActorLoc.Z;
   
   if (UNiagaraSystem* RangeEffect = GetOpeningNiagara())
   {
      if (!IsValid(RangeEffect)) return;
      
      SpawnedNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
          RangeEffect,
          GetRootComponent(),
          NAME_None,
          FVector(0.f, 0.f, GroundOffsetZ),
          FRotator::ZeroRotator,
          EAttachLocation::KeepRelativeOffset,
          false,
          true,
          ENCPoolMethod::AutoRelease,
          true
      );
   }

   if (AnimComponent && !AnimComponent->IsAnyMontagePlaying())
   {
      AnimComponent->PlayCombatMontage();
   }
}
