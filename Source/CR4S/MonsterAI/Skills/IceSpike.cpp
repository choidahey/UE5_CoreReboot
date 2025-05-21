#include "MonsterAI/Skills/IceSpike.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"

AIceSpike::AIceSpike()
	: LifeTime(5.f)
	, RiseDuration(0.5f)
	, RiseHeight(30.f)
	, PushHorizontalStrength(100.f)
	, PushUpStrength(0.f)
	, bRising(true)
	, ElapsedRise(0.f)
	, MyHeader(TEXT("IceSpike"))
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComp"));
	CollisionComp->SetupAttachment(RootComp);
	CollisionComp->SetBoxExtent(FVector(50.f, 50.f, 50.f));

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetGenerateOverlapEvents(true);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AIceSpike::OnOverlapBegin);
	

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComponent);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	InitialLifeSpan = LifeTime;
}

AIceSpike* AIceSpike::SpawnIceSpikeAtPlayer(APawn* OwnerPawn, TSubclassOf<AIceSpike> SpikeClass)
{
	if (!OwnerPawn || !SpikeClass) return nullptr;

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerPawn->GetWorld(), 0);
	if (!PlayerPawn) return nullptr;

	FVector PlayerLocation = PlayerPawn->GetActorLocation();

	// NOTICE :: Debug LineTrace
	FHitResult Hit;
	FVector StartLocation = PlayerLocation + FVector(0, 0, 200.f);
	FVector EndLocation = PlayerLocation - FVector(0, 0, 200.f);

	FCollisionQueryParams TraceParams(NAME_None);
	TraceParams.AddIgnoredActor(OwnerPawn);

	FVector SpawnLocation = PlayerLocation;
	if (OwnerPawn->GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECC_Visibility, TraceParams))
	{
		SpawnLocation = Hit.Location;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPawn;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	return OwnerPawn->GetWorld()->SpawnActor<AIceSpike>(SpikeClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
}

void AIceSpike::BeginPlay()
{
	Super::BeginPlay();

	InitialOffset = StaticMeshComp->GetRelativeLocation();
	TargetOffset = InitialOffset + FVector(0, 0, RiseHeight);

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ElapsedRise = 0.f;
	bRising = true;

}

void AIceSpike::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bRising) return;

	ElapsedRise += DeltaTime;
	float Alpha = FMath::Clamp(ElapsedRise / RiseDuration, 0.f, 1.f);

	FVector NewOffset = FMath::Lerp(InitialOffset, TargetOffset, Alpha);
	StaticMeshComp->SetRelativeLocation(NewOffset);
	CollisionComp->SetRelativeLocation(NewOffset);

	if (Alpha >= 1.f)
	{
		bRising = false;
	}
}

void AIceSpike::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Char = Cast<ACharacter>(OtherActor);
	if (!Char) return;

	CollisionComp->SetGenerateOverlapEvents(false);

	FVector Dir = (Char->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
	FVector LaunchVel = Dir * PushHorizontalStrength + FVector(0, 0, PushUpStrength);
	Char->LaunchCharacter(LaunchVel, true, true);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	float DamageAmount = 0.f;
	if (UMonsterSkillComponent* SkillComp = OwnerPawn->FindComponentByClass<UMonsterSkillComponent>())
	{
		DamageAmount = SkillComp->GetCurrentSkillData().Damage;
	}

	UGameplayStatics::ApplyDamage(
		OtherActor,
		DamageAmount,
		nullptr,
		OwnerPawn,
		UDamageType::StaticClass()
	);

	UE_LOG(LogTemp, Log, TEXT("[%s] Hit %s, Damage %0.2f applied"), *MyHeader, *OtherActor->GetName(), DamageAmount);
}


