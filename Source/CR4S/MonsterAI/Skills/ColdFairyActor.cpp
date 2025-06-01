#include "MonsterAI/Skills/ColdFairyActor.h"

#include "CR4S.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Kismet/KismetMathLibrary.h"

AColdFairyActor::AColdFairyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(20.f);
	CollisionComp->SetupAttachment(RootComp);
	
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &AColdFairyActor::OnHit);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetupAttachment(RootComp);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovementComp->UpdatedComponent = CollisionComp;
	ProjectileMovementComp->InitialSpeed = Speed;
	ProjectileMovementComp->MaxSpeed = MaxSpeed;
	ProjectileMovementComp->ProjectileGravityScale = 1.0f;
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->bShouldBounce = false;
	ProjectileMovementComp->bAutoActivate = false;
	ProjectileMovementComp->bIsHomingProjectile = false;
	ProjectileMovementComp->HomingAccelerationMagnitude = 0.f;
}

void AColdFairyActor::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* OwnerActor = GetOwner())
	{
		if (UMonsterSkillComponent* SkillComp = OwnerActor->FindComponentByClass<UMonsterSkillComponent>())
		{
			const FMonsterSkillData& SkillData = SkillComp->GetCurrentSkillData();
			Damage = SkillData.Damage;
		}
	}
}

void AColdFairyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bHasLaunched && IsValid(TargetActor))
	{
		FVector ToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		if (!ToTarget.IsNearlyZero())
		{
			SetActorRotation(ToTarget.Rotation());
		}
	}
}

void AColdFairyActor::InitialLaunch(AActor* InTarget, int32 InIndex, int32 TotalCount)
{
	if (!InTarget) return;
	
	TargetActor = InTarget;
	MySpawnOrder       = InIndex;
	ExpectedTotalCount = TotalCount;

	if (APawn* InstPawn = GetInstigator<APawn>())
	{
		CollisionComp->IgnoreActorWhenMoving(InstPawn, true);
	}
	
	if (bSequentialLaunch)
    {
        LaunchDelay = MySpawnOrder * Interval;

		if (LaunchDelay <= KINDA_SMALL_NUMBER)
		{
			LaunchSelf();
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(
				LaunchTimerHandle,
				this,
				&AColdFairyActor::Launch,
				LaunchDelay,
				false
			);
		}
    }
    else
    {
        TArray<AActor*> FoundAll;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AColdFairyActor::StaticClass(), FoundAll);

        TArray<AColdFairyActor*> FairyActors;
        for (AActor* Actor : FoundAll)
        {
            AColdFairyActor* CF = Cast<AColdFairyActor>(Actor);
            if (CF && CF->GetOwner() == GetOwner())
            {
                FairyActors.Add(CF);
            }
        }
    	
    	if (FairyActors.Num() < TotalCount) return;
    	
        for (AColdFairyActor* FairyActor : FairyActors)
        {
            if (IsValid(FairyActor))
            {
            	FairyActor->LaunchSelf();
            }
        }
    }
}

void AColdFairyActor::Launch()
{
	UE_LOG(LogTemp, Warning,
		TEXT("[%s][LaunchSelf] called  bHasLaunched=%s  TargetActor=%s"),
		*GetName(),
		bHasLaunched ? TEXT("true") : TEXT("false"),
		TargetActor ? *TargetActor->GetName() : TEXT("null")
	);
	
	if (!CR4S_VALIDATE(LogDa, IsValid(this))) return;
	if (!CR4S_VALIDATE(LogDa, IsValid(TargetActor))) return;
	if (bHasLaunched)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s][LaunchSelf] Aborting: already launched or invalid target."), *GetName());
		return;
	}
	bHasLaunched = true;
	
	if (!ProjectileMovementComp->IsActive())
	{
		ProjectileMovementComp->Activate(true);
		CR4S_Log(LogDa, Log, TEXT("[%s] LaunchSelf: ProjectileMovementComp activated manually"), *MyHeader);
	}
	UE_LOG(LogTemp, Warning,
		TEXT("[%s][LaunchSelf] IsActive() = %s"),
		*GetName(),
		ProjectileMovementComp->IsActive() ? TEXT("Yes") : TEXT("No")
	);
	
	ProjectileMovementComp->bIsHomingProjectile = true;
	ProjectileMovementComp->HomingAccelerationMagnitude = 5000.f;
	ProjectileMovementComp->HomingTargetComponent = TargetActor->GetRootComponent();
	
	FVector Direction = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	ProjectileMovementComp->Velocity = Direction * ProjectileMovementComp->InitialSpeed;
	
	CR4S_Log(LogDa, Log,
		TEXT("[%s] LaunchSelf: Homeing activated → Target=%s, InitialDir=%s"),
		*MyHeader,
		*TargetActor->GetName(),
		*Direction.ToString()
	);
}

void AColdFairyActor::OnHit(
    UPrimitiveComponent* HitComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning,
		TEXT("[%s][OnHit] OtherActor=%s  ImpactPoint=%s"),
		*GetName(),
		OtherActor ? *OtherActor->GetName() : TEXT("null"),
		*Hit.ImpactPoint.ToString()
	);
	
    if (!OtherActor || OtherActor == this || OtherActor == GetInstigator())
        return;
	
    UGameplayStatics::ApplyDamage(
        OtherActor,
        Damage,
        GetInstigatorController(),
        this,
        UDamageType::StaticClass()
    );
	
    ProjectileMovementComp->StopMovementImmediately();
    SetActorLocation(Hit.ImpactPoint);

    if (CollisionComp->IsRegistered())
    {
        CollisionComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    }
    AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);

    GetWorld()->GetTimerManager().SetTimer(
        DestroyTimerHandle,
        FTimerDelegate::CreateLambda([this]() { Destroy(); }),
        2.0f,
        false
    );
}
