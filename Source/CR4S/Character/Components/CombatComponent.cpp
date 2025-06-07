// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}

void UCombatComponent::SetTopSocketName(const FName InSocketName)
{
	TopSocketName=InSocketName;
}

void UCombatComponent::SetBottomSocketName(const FName InSocketName)
{
	BottomSocketName=InSocketName;
}

void UCombatComponent::SetInputEnable(const bool Enable)
{
	bInputEnable=Enable;
	if (Enable)
	{
		ExecuteInputQueue();
	}
}

void UCombatComponent::SetInputQueue(const EInputType Input)
{
	CurrentInputQueue=Input;

	if (BufferClearTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(BufferClearTimerHandle);
	}
	GetWorld()->GetTimerManager().SetTimer(
		BufferClearTimerHandle,
		this,
		&UCombatComponent::ClearInputQueue,
		1.f,
		false
	);
}

bool UCombatComponent::CheckInputQueue(const EInputType Input)
{
	if (bInputEnable) return true;

	SetInputQueue(Input);
	return false;
}

void UCombatComponent::SweepAndApplyDamage(AActor* OwningCharacter, const FVector& CurrentTop, const FVector& CurrentBottom, const float InDamage)
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
		ECC_Visibility,
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
			AlreadyDamagedActors.Add(HitActor);	
		}
	}

	PreviousTopLocation=CurrentTop;
	PreviousBottomLocation=CurrentBottom;

	if (!bDebugMode) return;
	const FVector BoxCenter = CurrentBottom + Delta * 0.5f;
	DrawDebugBox(GetWorld(), BoxCenter, BoxHalfSize, Look.Quaternion(), FColor::Red, false, 2.f);
}

void UCombatComponent::SetWeaponTrace(const bool Trace)
{
}

void UCombatComponent::ExecuteInputQueue()
{
}

void UCombatComponent::PerformWeaponTrace()
{
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatComponent::ClearInputQueue()
{
	CurrentInputQueue=EInputType::None;
	BufferClearTimerHandle.Invalidate();
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	if (bWeaponTrace)
	{
		PerformWeaponTrace();		
	}
}

