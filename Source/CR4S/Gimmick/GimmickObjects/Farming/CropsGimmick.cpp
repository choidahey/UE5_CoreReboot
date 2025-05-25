#include "CropsGimmick.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Inventory/InventoryComponent.h"

ACropsGimmick::ACropsGimmick()
	: HarvestText(FText::FromString(TEXT("수확 하기"))),
	  DetectingActor(nullptr),
	  bIsDetected(false),
	  bIsHarvestable(false),
	  GrowthPercentPerInterval(10.f),
	  IntervalSeconds(5.f),
	  PreviousGrowthStage(0),
	  CurrentGrowthPercent(0.f),
	  MaxGrowthPercent(100.f)
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
}

void ACropsGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnDetectionStateChanged.BindDynamic(this, &ThisClass::OnDetectionStateChanged);
		InteractableComponent->OnTryInteract.BindDynamic(this, &ThisClass::OnGimmickInteracted);

		DefaultInteractionText = InteractableComponent->GetInteractionText();
	}

	GrowthPercentPerInterval = 10.f;
	IntervalSeconds = 2.f;

	GrowthStageThresholds = {25.f, 50.f, 75.f, 100.f};
	GrowthStageScale = {0.1f, 0.25f, 0.5f, 0.75f, 1.f};

	MaxGrowthPercent = GrowthStageThresholds.Num() != 0 ? GrowthStageThresholds.Last() : 100.f;

	if (!bIsHarvestable)
	{
		GetWorldTimerManager().SetTimer(
			GrowthTimerHandle,
			this,
			&ThisClass::Grow,
			IntervalSeconds,
			true
		);
	}
}

void ACropsGimmick::OnGimmickInteracted(AActor* Interactor)
{
	GetResources(Interactor);
}

void ACropsGimmick::OnDetectionStateChanged(AActor* InDetectingActor, const bool bInIsDetected)
{
	DetectingActor = InDetectingActor;
	bIsDetected = bInIsDetected;

	if (Cast<APlayerController>(DetectingActor) && bIsDetected)
	{
		UpdateInteractionText();
	}
}

void ACropsGimmick::UpdateInteractionText() const
{
	if (IsValid(InteractableComponent))
	{
		const FString InteractionString = DefaultInteractionText.ToString()
			+ FString::Printf(TEXT(" %.f%%"), CurrentGrowthPercent);

		FText Text = FText::FromString(InteractionString);

		if (bIsHarvestable)
		{
			Text = HarvestText;
		}

		InteractableComponent->SetInteractionText(Text);
	}
}

void ACropsGimmick::GrowthStageChanged(const int32 NewGrowthStage)
{
	if (IsValid(GimmickMeshComponent) && GrowthStageScale.IsValidIndex(NewGrowthStage))
	{
		const FVector NewScale = FVector(GrowthStageScale[NewGrowthStage]);
		GimmickMeshComponent->SetRelativeScale3D(NewScale);
		
		if (NewGrowthStage == GrowthStageThresholds.Num())
		{
			GetWorldTimerManager().ClearTimer(GrowthTimerHandle);
			bIsHarvestable = true;
		}
	}
}

bool ACropsGimmick::IsHeldItemSeed() const
{
	if (!IsValid(DetectingActor) || !bIsDetected)
	{
		UE_LOG(LogTemp, Warning, TEXT("DetectingController is not valid or bIsDetected is false"));
		return false;
	}

	const UInventoryComponent* InventorySystem
		= DetectingActor->FindComponentByClass<UInventoryComponent>();

	if (!IsValid(InventorySystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySystem is not valid"));
		return false;
	}

	return InventorySystem->GetCurrentHeldItemName() == FName("Seed");
}

void ACropsGimmick::Grow()
{
	CurrentGrowthPercent = FMath::Clamp(CurrentGrowthPercent + GrowthPercentPerInterval, 0.f, MaxGrowthPercent);

	if (Cast<APlayerController>(DetectingActor) && bIsDetected)
	{
		UpdateInteractionText();
	}

	const int32 NewStage = CalculateGrowthStage();
	if (NewStage != PreviousGrowthStage)
	{
		PreviousGrowthStage = NewStage;
		if (NewStage >= 0)
		{
			GrowthStageChanged(NewStage);
		}
	}
}

int32 ACropsGimmick::CalculateGrowthStage() const
{
	for (int32 Index = GrowthStageThresholds.Num() - 1; Index >= 0; Index--)
	{
		if (CurrentGrowthPercent >= GrowthStageThresholds[Index])
		{
			return Index + 1;
		}
	}
	
	return 0;
}
