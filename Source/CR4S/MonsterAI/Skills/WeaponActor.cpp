#include "MonsterAI/Skills/WeaponActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"

AWeaponActor::AWeaponActor()
{
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollision"));
	CollisionComp->SetupAttachment(RootComp);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->SetGenerateOverlapEvents(false);

	CollisionComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &AWeaponActor::OnOverlap);
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	SetupIgnoreOwnerCollision();
}

void AWeaponActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeaponActor::SetupIgnoreOwnerCollision()
{
	if (AActor* MyOwner = GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(MyOwner, true);

		TArray<UPrimitiveComponent*> OwnerComps;
		MyOwner->GetComponents<UPrimitiveComponent>(OwnerComps);
		for (UPrimitiveComponent* OwnerComp : OwnerComps)
		{
			if (OwnerComp)
			{
				CollisionComp->IgnoreComponentWhenMoving(OwnerComp, true);
				OwnerComp->IgnoreComponentWhenMoving(CollisionComp, true);
			}
		}
	}
}

void AWeaponActor::ActivateWeaponCollision()
{
	UpdateSkillDataFromOwner();
	if (!bShouldActivateCollision) return;

	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);
	AlreadyDamaged.Empty();
}

void AWeaponActor::DeactivateWeaponCollision()
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->SetGenerateOverlapEvents(false);
}

void AWeaponActor::UpdateSkillDataFromOwner()
{
	if (AActor* MyOwner = GetOwner())
	{
		if (UMonsterSkillComponent* SkillComp = MyOwner->FindComponentByClass<UMonsterSkillComponent>())
		{
			const FMonsterSkillData& SkillData = SkillComp->GetCurrentSkillData();

			if (!SkillData.bUseWeaponCollision)
			{
				bShouldActivateCollision = false;
				return;
			}

			Damage = SkillData.Damage;
			StunGaugeAmount = SkillData.StunGauge;
			bAllowMultipleHits = SkillData.bAllowMultipleHits;
			bShouldActivateCollision = true;
		}
	}
}
