#include "RegionBossMonster.h"
#include "CR4S/MonsterAI/Components/MonsterStateComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationInvokerComponent.h"

ARegionBossMonster::ARegionBossMonster()
	: MyHeader(TEXT("RegionBossMonster"))
{
	PrimaryActorTick.bCanEverTick = false;

	NavGenerationRadius = 200.0f;
	NavRemovalRadius = 250.0f;

	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
	NavInvoker->SetGenerationRadii(NavGenerationRadius, NavRemovalRadius);
}

void ARegionBossMonster::OnIdleBoredNotify()
{
	if (IsDead() || IdleBoredMontages.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] IdleBoredNotify skipped: Monster is dead or montage list is empty."), *MyHeader);
		return;
	}

	if (USkeletalMeshComponent* SkeletalMeshComp = GetMesh())
	{
		if (UAnimInstance* Anim = SkeletalMeshComp->GetAnimInstance())
		{
			if (Anim->IsAnyMontagePlaying())
			{
				UE_LOG(LogTemp, Warning, TEXT("[%s] IdleBoredNotify skipped: Another montage is currently playing."), *MyHeader);
				return;
			}

			const int32 Index = FMath::RandRange(0, IdleBoredMontages.Num() - 1);
			if (UAnimMontage* Montage = IdleBoredMontages[Index])
			{
				UE_LOG(LogTemp, Log, TEXT("[%s] Playing idle bored montage: %s"), *MyHeader, *Montage->GetName());
				Anim->Montage_Play(Montage);
			}
		}
	}
}
