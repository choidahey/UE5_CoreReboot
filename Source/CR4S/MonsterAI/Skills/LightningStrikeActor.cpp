#include "MonsterAI/Skills/LightningStrikeActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "DrawDebugHelpers.h"

ALightningStrikeActor::ALightningStrikeActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LightningEffect"));
	NiagaraComp->SetupAttachment(RootComp);
	NiagaraComp->SetAutoActivate(false);

	LightningCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LightningCollider"));
	LightningCollider->SetupAttachment(RootComp);

	LightningCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	LightningCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LightningCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	LightningCollider->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ALightningStrikeActor::InitializeStrike(const FVector& TargetLocation, UNiagaraSystem* LightningEffect, float DamageAmount)
{
	TargetPosition = TargetLocation;
	Damage = DamageAmount;

	if (LightningEffect)
	{
		NiagaraComp->SetAsset(LightningEffect);
		NiagaraComp->Activate(true);
	}

	const FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
	SetActorRotation(Direction.Rotation());

	if (IsValid(LightningCollider))
	{
		LightningCollider->SetCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
		LightningCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		LightningCollider->OnComponentBeginOverlap.AddDynamic(this, &ALightningStrikeActor::OnOverlap);

		LightningCollider->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void ALightningStrikeActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALightningStrikeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(LightningCollider))
	{
		UE_LOG(LogTemp, Warning, TEXT("[LightningStrikeActor] LightningCollider is null in Tick."));
		return;
	}

	const FVector Current = LightningCollider->GetComponentLocation();
	const FVector ToTarget = (TargetPosition + FVector(0, 0, CapsuleHalfHeight)) - Current;
	const FVector Move = ToTarget.GetClampedToMaxSize(CapsuleSpeed * DeltaTime);
	const FVector NewLocation = Current + Move;

	LightningCollider->SetWorldLocation(NewLocation);

	DrawDebugCapsule(
		GetWorld(),
		NewLocation,
		CapsuleHalfHeight,
		CapsuleRadius,
		LightningCollider->GetComponentQuat(),
		FColor::Red,
		false,
		-1.f,
		0,
		2.f
	);

	if (ToTarget.SizeSquared() < FMath::Square(ArrivalThreshold))
	{
		Destroy();
	}
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
