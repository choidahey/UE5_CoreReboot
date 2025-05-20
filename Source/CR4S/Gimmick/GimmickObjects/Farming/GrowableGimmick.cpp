#include "GrowableGimmick.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventorySystemComponent.h"

AGrowableGimmick::AGrowableGimmick()
	: HarvestText(FText::FromString(TEXT("수확 하기"))),
	  DetectingPlayerController(nullptr),
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

void AGrowableGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnDetectionStateChanged.BindUObject(this, &ThisClass::OnDetectionStateChanged);
		InteractableComponent->OnTryInteract.BindUObject(this, &ThisClass::OnGimmickInteracted);

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

void AGrowableGimmick::OnGimmickInteracted()
{
	if (!IsValid(InteractableComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractableComponent is not valid"));
		return;
	}

	if (!IsValid(DetectingPlayerController))
	{
		UE_LOG(LogTemp, Warning, TEXT("DetectingPlayerController is not valid"));
		return;
	}

	if (!bIsHarvestable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Is not harvestable"));
		return;
	}

	const APawn* DetectingPawn = DetectingPlayerController->GetPawn();
	if (!IsValid(DetectingPawn))
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is not valid"));
		return;
	}

	UInventorySystemComponent* InventorySystem
		= DetectingPawn->FindComponentByClass<UInventorySystemComponent>();
	if (!IsValid(InventorySystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySystem is not valid"));
		return;
	}

	const UItemGimmickSubsystem* ItemGimmickSubsystem = GetGameInstance()->GetSubsystem<UItemGimmickSubsystem>();
	if (!IsValid(ItemGimmickSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemGimmickSubsystem is not valid"));
		return;
	}

	if (const FBaseGimmickData* GimmickData = ItemGimmickSubsystem->FindGimmickData(GetGimmickDataRowName()))
	{
		for (const auto& [RowName, Count] : GimmickData->ResourceItemDataList)
		{
			const FBaseItemData* ItemData = ItemGimmickSubsystem->FindItemData(RowName);
			if (!ItemData)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s is not found in ItemData"), *RowName.ToString());
				continue;
			}

			const FAddItemResult Result
				= InventorySystem->AddItem(FInventoryItem(RowName, ItemData->Icon.Get(), Count));

			UE_LOG(LogTemp, Warning, TEXT("Success: %d / AddCount: %d / RemainingCount: %d")
				   , Result.Success, Result.AddedCount, Result.RemainingCount);
			
			if (Result.Success)
			{
				OnHarvest.ExecuteIfBound();
				
				Destroy();
			}
		}
	}
}

void AGrowableGimmick::OnDetectionStateChanged(APlayerController* InDetectingPlayerController, const bool bInIsDetected)
{
	DetectingPlayerController = InDetectingPlayerController;
	bIsDetected = bInIsDetected;

	if (IsValid(DetectingPlayerController) && bIsDetected)
	{
		UpdateInteractionText();
	}
}

void AGrowableGimmick::UpdateInteractionText() const
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

void AGrowableGimmick::GrowthStageChanged(const int32 NewGrowthStage)
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

bool AGrowableGimmick::IsHeldItemSeed() const
{
	if (!IsValid(DetectingPlayerController) || !bIsDetected)
	{
		UE_LOG(LogTemp, Warning, TEXT("DetectingPlayerController is not valid or bIsDetected is false"));
		return false;
	}

	const UInventorySystemComponent* InventorySystem
		= DetectingPlayerController->FindComponentByClass<UInventorySystemComponent>();

	if (!IsValid(InventorySystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySystem is not valid"));
		return false;
	}

	return InventorySystem->GetCurrentHeldItemName() == FName("Seed");
}

void AGrowableGimmick::Grow()
{
	CurrentGrowthPercent = FMath::Clamp(CurrentGrowthPercent + GrowthPercentPerInterval, 0.f, MaxGrowthPercent);

	if (IsValid(DetectingPlayerController) && bIsDetected)
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

int32 AGrowableGimmick::CalculateGrowthStage() const
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
