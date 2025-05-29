#include "MonsterAI/Skills/LightningStrikeActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"

ALightningStrikeActor::ALightningStrikeActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LightningEffect"));
	NiagaraComp->SetupAttachment(RootComp);
	NiagaraComp->SetAutoActivate(false);

	LightningCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LightningCollider"));
	LightningCollider->SetupAttachment(RootComp);

	LightningCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	LightningCollider->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
	LightningCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LightningCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	LightningCollider->SetGenerateOverlapEvents(true);

	LightningCollider->OnComponentBeginOverlap.AddDynamic(this, &ALightningStrikeActor::OnOverlap);
}

void ALightningStrikeActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALightningStrikeActor::InitializeStrike(const FVector& TargetLocation, UNiagaraSystem* LightningEffect, float DamageAmount)
{
	Damage = DamageAmount;
	SetActorLocation(TargetLocation);

	if (LightningEffect)
	{
		NiagaraComp->SetAsset(LightningEffect);
		NiagaraComp->SetWorldScale3D(FVector(10.f));
		NiagaraComp->SetVariableFloat(FName("User_Width"), LightningWidth);
		NiagaraComp->Activate(true);
	}

	if (IsValid(LightningCollider))
	{
		TArray<AActor*> OverlappingActors;
		LightningCollider->GetOverlappingActors(OverlappingActors, APawn::StaticClass());

		for (AActor* Overlapped : OverlappingActors)
		{
			FHitResult DummyHit;

			OnOverlap(
				LightningCollider,
				Overlapped,
				nullptr,
				INDEX_NONE,
				false,
				DummyHit
			);
		}

		DrawDebugCapsule(
			GetWorld(),
			LightningCollider->GetComponentLocation(),
			CapsuleHalfHeight,
			CapsuleRadius,
			LightningCollider->GetComponentQuat(),
			FColor::Red,
			false,
			2.f,
			0,
			2.f
		);
	}

	FTimerHandle DestroyHandle;
	GetWorld()->GetTimerManager().SetTimer(
		DestroyHandle,
		this,
		&ALightningStrikeActor::HandleSelfDestruct,
		LightningDelayBeforeStrike + ActorLifeAfterStrike,
		false
	);
}

void ALightningStrikeActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetInstigator() || OtherActor == GetOwner()) return;
	if (OtherActor->FindComponentByClass<UMonsterAttributeComponent>()) return;
	if (DamagedActors.Contains(OtherActor)) return;

	UGameplayStatics::ApplyDamage(
		OtherActor,
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);

	DamagedActors.Add(OtherActor);
}

void ALightningStrikeActor::HandleSelfDestruct()
{
	Destroy();
}
