// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent():
	bInputEnable(true),
	bWeaponTrace(false),
	PreviousTopLocation(FVector()),
	PreviousBottomLocation(FVector()),
	CurrentInputQueue(EInputType::None)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}

void UCombatComponent::Input_OnAttack()
{
	if (!CheckInputQueue(EInputType::Attack)) return;
	OwningCharacter->PlayAnimMontage(AttackMontage);
}

void UCombatComponent::PerformWeaponTrace()
{
	if (!bWeaponTrace) return;
	UStaticMeshComponent* Weapon=OwningCharacter->GetOverlayStaticMesh();
	if (!Weapon) return;
	//Socket Location
	FVector CurrentTop=Weapon->GetSocketLocation("Top");
	FVector CurrentBottom=Weapon->GetSocketLocation("Bottom");
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

	const FVector BoxCenter = CurrentBottom + Delta * 0.5f;
	//DrawDebugBox(GetWorld(), BoxCenter, BoxHalfSize, Look.Quaternion(), FColor::Red, false, 2.f);
}

void UCombatComponent::SetInputEnable(bool Enable)
{
	bInputEnable=Enable;
	if (Enable)
	{
		ExecuteInputQueue();
	}
}

void UCombatComponent::SetWeaponTrace(bool Trace)
{
	AlreadyDamagedActors.Empty();
	bWeaponTrace=Trace;
	if (!Trace) return;
	UStaticMeshComponent* Weapon=OwningCharacter->GetOverlayStaticMesh();
	if (!Weapon) return;
	PreviousTopLocation=Weapon->GetSocketLocation("Top");
	PreviousBottomLocation=Weapon->GetSocketLocation("Bottom");
	
}

void UCombatComponent::SetInputQueue(EInputType Input)
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

bool UCombatComponent::CheckInputQueue(EInputType Input)
{
	if (bInputEnable) return true;

	SetInputQueue(Input);
	return false;
}

void UCombatComponent::ExecuteInputQueue()
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


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwningCharacter=Cast<APlayerCharacter>(GetOwner());
	
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
	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

