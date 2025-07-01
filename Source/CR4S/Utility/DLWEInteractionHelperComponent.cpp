#include "DLWEInteractionHelperComponent.h"

#include "CR4S.h"

UDLWEInteractionHelperComponent::UDLWEInteractionHelperComponent()
	: MaxLegCount(0),
	  SocketPrefix(FName("Feet")),
	  bIsInitialized(false)
{
}

void UDLWEInteractionHelperComponent::BeginPlay()
{
	Super::BeginPlay();

	TryCreateDLWEInteractionComponent();
}

void UDLWEInteractionHelperComponent::CreateDLWEInteractionComponent()
{
	if (bIsInitialized)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!CR4S_VALIDATE(LogUtility, IsValid(Owner)) ||
		!CR4S_VALIDATE(LogUtility, DLWEInteractionClass))
	{
		return;
	}

	LegMesh = Cast<USkeletalMeshComponent>(GetAttachParent());
	if (!CR4S_VALIDATE(LogUtility, IsValid(LegMesh)))
	{
		return;
	}

	ClearDLWEInteractionComponents();

	DLWEInteractionInstances.SetNum(MaxLegCount);

	for (int32 Index = 0; Index < MaxLegCount; Index++)
	{
		USceneComponent* Component = NewObject<USceneComponent>(Owner, DLWEInteractionClass);
		if (CR4S_VALIDATE(LogUtility, IsValid(Component)))
		{
			Component->RegisterComponent();
			DLWEInteractionInstances[Index] = Component;

			if (!Attach(Index, Component))
			{
				CR4S_Log(LogUtility, Error, TEXT("Failed to attach component at index %d"), Index);
				Component->DestroyComponent();
				DLWEInteractionInstances[Index] = nullptr;
			}
		}
		else
		{
			DLWEInteractionInstances[Index] = nullptr;
		}
	}
}

void UDLWEInteractionHelperComponent::UpdateConnectSlot()
{
	if (!CR4S_VALIDATE(LogUtility, IsValid(LegMesh)))
	{
		return;
	}

	int32 Index = 0;
	for (USceneComponent* Component : DLWEInteractionInstances)
	{
		if (IsValid(Component))
		{
			Component->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

			Attach(Index, Component);
			Index++;
		}
	}
}

bool UDLWEInteractionHelperComponent::IsReadyToCreate() const
{
	const USkeletalMeshComponent* TempLegMesh = Cast<USkeletalMeshComponent>(GetAttachParent());
	if (!IsValid(TempLegMesh))
	{
		return false;
	}

	if (!TempLegMesh->GetSkeletalMeshAsset())
	{
		return false;
	}

	for (int32 Index = 0; Index < MaxLegCount; Index++)
	{
		const FName SocketName = CreateSocketName(Index);
		if (!TempLegMesh->DoesSocketExist(SocketName))
		{
			CR4S_Log(LogUtility, Warning, TEXT("Required socket [%s] does not exist"), *SocketName.ToString());
			return false;
		}
	}

	return true;
}

void UDLWEInteractionHelperComponent::TryCreateDLWEInteractionComponent()
{
	if (bIsInitialized)
	{
		return;
	}

	if (!IsReadyToCreate())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::TryCreateDLWEInteractionComponent);
		return;
	}

	CreateDLWEInteractionComponent();
	bIsInitialized = true;
}

void UDLWEInteractionHelperComponent::ClearDLWEInteractionComponents()
{
	for (USceneComponent* Component : DLWEInteractionInstances)
	{
		if (IsValid(Component))
		{
			Component->DestroyComponent();
		}
	}

	DLWEInteractionInstances.Empty();
}

bool UDLWEInteractionHelperComponent::Attach(const int32 Index, USceneComponent* Component) const
{
	if (!CR4S_VALIDATE(LogUtility, IsValid(LegMesh)) ||
		!CR4S_VALIDATE(LogUtility, IsValid(Component)))
	{
		return false;
	}

	const FName SocketName = CreateSocketName(Index);
	if (!LegMesh->DoesSocketExist(SocketName))
	{
		CR4S_Log(LogUtility, Error, TEXT("Socket [%s] does not exist"), *SocketName.ToString());
		return false;
	}

	Component->AttachToComponent(LegMesh, FAttachmentTransformRules::KeepRelativeTransform, SocketName);
	return true;
}

FName UDLWEInteractionHelperComponent::CreateSocketName(const int32 Index) const
{
	return FName(*FString::Printf(TEXT("%s_%d"), *SocketPrefix.ToString(), Index));
}
