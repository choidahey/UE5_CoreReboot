// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTraceComponent.h"

#include "CR4S.h"
#include "Character/Weapon/BaseTool.h"
#include "Character/Weapon/RobotWeapon/MeleeWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utility/CombatStatics.h"


// Sets default values for this component's properties
UWeaponTraceComponent::UWeaponTraceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UWeaponTraceComponent::SetCurrentTool(ABaseTool* InTool)
{
	CurrentTool = InTool;
}

void UWeaponTraceComponent::PerformWeaponTrace()
{
	if (!bWeaponTrace||!CurrentTool) return;
	
	UMeshComponent* MeshComp=CurrentTool->GetToolMeshComponent();
	if (!CR4S_ENSURE(LogHong1,MeshComp)) return;
	
	//Socket Location
	FVector CurrentTop=MeshComp->GetSocketLocation(TopSocketName);
	FVector CurrentBottom=MeshComp->GetSocketLocation(BottomSocketName);
	if (CurrentTop.IsZero() || CurrentBottom.IsZero()) return;
	
	const float Damage=CurrentTool->ComputeFinalDamage();
	AActor* OwningCharacter=CurrentTool->GetToolOwner();
	if (!CR4S_ENSURE(LogHong1,OwningCharacter)) return;
	
	SweepAndApplyDamage(OwningCharacter,CurrentTop,CurrentBottom,Damage);
}

void UWeaponTraceComponent::SweepAndApplyDamage(AActor* OwningCharacter, const FVector& CurrentTop,
	const FVector& CurrentBottom, const float InDamage)
{
	//Get Distance between Top and Bottom
	FVector Delta=CurrentTop-CurrentBottom;
	float Dist=Delta.Size();
	FVector BoxHalfSize(Dist*0.5f,10,10);
	//Set Orientation
	FRotator Look=UKismetMathLibrary::FindLookAtRotation(CurrentTop,CurrentBottom);
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
		ECC_GameTraceChannel15,
		FCollisionShape::MakeBox(BoxHalfSize),
		QueryParams
	);
	//Result process
	if (bHit)
	{
		for (const FHitResult& Hit: HitResults)
		{
			AActor* HitActor=Hit.GetActor();
			if (!IsValid(HitActor)||AlreadyDamagedActors.Contains(HitActor)) continue;
			
			AController* Instigator=OwningCharacter->GetInstigatorController();
			if (!IsValid(Instigator)) continue;
			
			UGameplayStatics::ApplyDamage(
				HitActor,
				InDamage,
				Instigator,
				OwningCharacter,
				UDamageType::StaticClass()
			);
			if (AMeleeWeapon* CurrentWeapon=Cast<AMeleeWeapon>(CurrentTool))
			{
				UCombatStatics::ApplyStun(HitActor,CurrentWeapon->GetStunAmount());
			}
			AlreadyDamagedActors.Add(HitActor);	
		}
	}

	PreviousTopLocation=CurrentTop;
	PreviousBottomLocation=CurrentBottom;

	if (!bDebugMode) return;
	const FVector BoxCenter = CurrentBottom + Delta * 0.5f;
	DrawDebugBox(GetWorld(), BoxCenter, BoxHalfSize, Look.Quaternion(), FColor::Red, false, 2.f);
}

void UWeaponTraceComponent::SetWeaponTrace(const bool Trace)
{
	AlreadyDamagedActors.Empty();
	bWeaponTrace=Trace;
	if (!Trace||!CurrentTool) return;
	
	UMeshComponent* MeshComp=CurrentTool->GetToolMeshComponent();
	if (!MeshComp) return;
	
	PreviousTopLocation=MeshComp->GetSocketLocation(TopSocketName);
	PreviousBottomLocation=MeshComp->GetSocketLocation(BottomSocketName);
}

void UWeaponTraceComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentTool=Cast<ABaseTool>(GetOwner());
}


void UWeaponTraceComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	if (bWeaponTrace)
	{
		PerformWeaponTrace();		
	}
}

