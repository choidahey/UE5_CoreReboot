#include "MonsterAI/Skills/WeaponActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AWeaponActor::AWeaponActor()
{
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollision"));
	CollisionComp->SetupAttachment(RootComp);

	CollisionComp->SetCollisionProfileName(TEXT("MonsterSkillActor"));
	CollisionComp->SetGenerateOverlapEvents(true);
}

void AWeaponActor::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
