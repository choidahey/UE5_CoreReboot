#include "MonsterAI/Skills/CubeDamageSkillActor.h"
#include "Components/BoxComponent.h"

ACubeDamageSkillActor::ACubeDamageSkillActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	CollisionComp->SetupAttachment(RootComponent);
}

void ACubeDamageSkillActor::BeginPlay()
{
	Super::BeginPlay();

	if (UBoxComponent* Box = Cast<UBoxComponent>(CollisionComp))
	{
		Box->SetBoxExtent(BoxExtent);
		Box->SetCollisionProfileName(TEXT("MonsterSkillActor"));
		Box->SetGenerateOverlapEvents(true);

		Box->OnComponentBeginOverlap.AddDynamic(this, &ACubeDamageSkillActor::OnOverlap);
	}

#if WITH_EDITOR
	DrawDebugBox(
		GetWorld(),
		GetActorLocation(),
		BoxExtent,
		FColor::Red,
		false,
		2.0f
	);
#endif

	ApplyInitialOverlapDamage();
	SetLifeSpan(LifeTime);
}

void ACubeDamageSkillActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ApplyEffectToActor(OtherActor);
}
