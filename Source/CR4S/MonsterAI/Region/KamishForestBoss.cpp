#include "KamishForestBoss.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AKamishForestBoss::AKamishForestBoss()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKamishForestBoss::BeginPlay()
{
	Super::BeginPlay();

	// Hide Weapon Bone
	if (USkeletalMeshComponent* SeletalMesh = GetMesh())
	{
		SeletalMesh->HideBoneByName(TEXT("Weapon_L"), EPhysBodyOp::PBO_Term);
		SeletalMesh->HideBoneByName(TEXT("Weapon_R"), EPhysBodyOp::PBO_Term);
	}

	// 2. Attach Weapon Actor
	AttachWeaponActor();
}

void AKamishForestBoss::OnMonsterStateChanged(EMonsterState Previous, EMonsterState Current)
{
	Super::OnMonsterStateChanged(Previous, Current);

	if (Current == EMonsterState::Combat)
	{
		SpawnCloudEffect();
		SkillComponent->UseSkill(0);
	}

	if (Previous == EMonsterState::Combat && Current != EMonsterState::Combat)
	{
		DestroyActiveClouds();
	}
}

void AKamishForestBoss::HandleDeath()
{
	DestroyActiveClouds();
	Super::HandleDeath();
}

void AKamishForestBoss::AttachWeaponActor()
{
	if (!WeaponActorClass) return;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UWorld* World = GetWorld();
	if (!World) return;

	AActor* LeftWeapon = World->SpawnActor<AActor>(WeaponActorClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
	AActor* RightWeapon = World->SpawnActor<AActor>(WeaponActorClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

	if (USkeletalMeshComponent* SeletalMesh = GetMesh())
	{
		if (LeftWeapon)
		{
			LeftWeapon->AttachToComponent(SeletalMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("AxeWeapon_L"));
		}
		if (RightWeapon)
		{
			RightWeapon->AttachToComponent(SeletalMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("AxeWeapon_R"));
		}
	}
}

void AKamishForestBoss::SpawnCloudEffect()
{
	if (!CloudAsset || ActiveCloudEffect) return;

	const FVector SpawnLocation = GetActorLocation() + FVector(0, 0, CloudVerticalOffset);

	ActiveCloudEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		CloudAsset,
		SpawnLocation,
		GetActorRotation(),
		FVector(1.f),
		true,
		true,
		ENCPoolMethod::None,
		true
	);

	if (ActiveCloudEffect)
	{
		const float SizeParam = CloudVisualRadius / NiagaraRadiusBase;
		// ActiveCloudEffect->SetVariableFloat(FName("User_CloudSize"), SizeParam);
	}
}

void AKamishForestBoss::DestroyActiveClouds()
{
	if (IsValid(ActiveCloudEffect))
	{
		ActiveCloudEffect->DestroyComponent();
		ActiveCloudEffect = nullptr;
	}
}

