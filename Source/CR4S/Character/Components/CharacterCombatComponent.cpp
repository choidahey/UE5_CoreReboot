// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCombatComponent.h"

#include "CR4S.h"
#include "PlayerCharacterStatusComponent.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UCharacterCombatComponent::UCharacterCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UCharacterCombatComponent::Input_OnAttack()
{
	if (!CheckInputQueue(EInputType::Attack)) return;
	UStaticMesh* ToolMesh=OwningCharacter->GetToolStaticMesh();
	if (!CR4S_ENSURE(LogHong1,ToolMesh)) return;
	OwningCharacter->PlayAnimMontage(AttackMontage);
}

void UCharacterCombatComponent::PerformWeaponTrace()
{
	if (!bWeaponTrace) return;
	UStaticMeshComponent* Weapon=OwningCharacter->GetOverlayStaticMesh();
	if (!Weapon) return;
	UStaticMesh* ToolMesh=OwningCharacter->GetToolStaticMesh();
	if (!CR4S_ENSURE(LogHong1,ToolMesh)) return;
	//Socket Location
	FVector CurrentTop=Weapon->GetSocketLocation(TopSocketName);
	FVector CurrentBottom=Weapon->GetSocketLocation(BottomSocketName);
	//Get Distance between Top and Bottom
	FVector Delta=CurrentTop-CurrentBottom;
	float Dist=Delta.Size();
	//Set BoxHalfSize
	FVector BoxHalfSize(Dist*0.5f,10,10);
	//Set Orientation
	FRotator Look=UKismetMathLibrary::FindLookAtRotation(CurrentTop,CurrentBottom);
	//Query
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex=true;
	QueryParams.AddIgnoredActor(OwningCharacter);
	//Box Trace by Multi
	TArray<FHitResult> HitResults;
	bool bHit=GetWorld()->SweepMultiByChannel(
		HitResults,
		PreviousTopLocation,
		CurrentTop,
		Look.Quaternion(),
		ECC_Visibility,
		FCollisionShape::MakeBox(BoxHalfSize),
		QueryParams
	);
	float Damage=0;
	if (UPlayerCharacterStatusComponent* StatusComp=OwningCharacter->FindComponentByClass<UPlayerCharacterStatusComponent>())
	{
		Damage=StatusComp->GetAttackPower();
	}
	//Result process
	if (bHit)
	{
		for (const FHitResult& Hit: HitResults)
		{
			if (AActor* HitActor=Hit.GetActor())
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *HitActor->GetName());
				if (!(AlreadyDamagedActors.Contains(HitActor)))
				{
					UE_LOG(LogTemp, Warning, TEXT("Applying damage to: %s"), *HitActor->GetName());
					UGameplayStatics::ApplyDamage(
						HitActor,
						Damage,
						OwningCharacter->GetController(),
						OwningCharacter,
						UDamageType::StaticClass()
					);
					AlreadyDamagedActors.Add(HitActor);
				}
			}
		}
	}

	PreviousTopLocation=CurrentTop;
	PreviousBottomLocation=CurrentBottom;

	if (!bDebugMode) return;
	const FVector BoxCenter = CurrentBottom + Delta * 0.5f;
	DrawDebugBox(GetWorld(), BoxCenter, BoxHalfSize, Look.Quaternion(), FColor::Red, false, 2.f);
}

void UCharacterCombatComponent::SetWeaponTrace(const bool Trace)
{
	AlreadyDamagedActors.Empty();
	bWeaponTrace=Trace;
	if (!Trace) return;
	UStaticMeshComponent* Weapon=OwningCharacter->GetOverlayStaticMesh();
	if (!Weapon) return;
	UStaticMesh* ToolMesh=Weapon->GetStaticMesh();
	if (!CR4S_ENSURE(LogHong1,ToolMesh)) return;
	PreviousTopLocation=Weapon->GetSocketLocation(TopSocketName);
	PreviousBottomLocation=Weapon->GetSocketLocation(BottomSocketName);
}

void UCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningCharacter=Cast<APlayerCharacter>(GetOwner());
}

void UCharacterCombatComponent::ExecuteInputQueue()
{
	switch (CurrentInputQueue)
	{
	case EInputType::None:
		break;
	case EInputType::Attack:
		Input_OnAttack();
		break;
	default:
		break;
	}
}

void UCharacterCombatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	if (bWeaponTrace)
	{
		PerformWeaponTrace();		
	}
}
