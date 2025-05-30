#include "CropsGimmick.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Gimmick/Components/InteractableComponent.h"

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

	if (CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
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
	if (!CR4S_VALIDATE(LogGimmick, bIsHarvestable))
	{
		return;
	}
	
	Harvest(Interactor);
}

void ACropsGimmick::OnDetectionStateChanged(AActor* InDetectingActor, const bool bInIsDetected)
{
	DetectingActor = InDetectingActor;
	bIsDetected = bInIsDetected;

	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(DetectingActor);
	if (IsValid(PlayerCharacter) && bIsDetected)
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

void ACropsGimmick::Harvest(const AActor* Interactor)
{
	OnHarvest.ExecuteIfBound();
	
	GetResources(Interactor);

	GimmickDestroy();
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

void ACropsGimmick::Grow()
{
	CurrentGrowthPercent = FMath::Clamp(CurrentGrowthPercent + GrowthPercentPerInterval, 0.f, MaxGrowthPercent);

	if (Cast<APlayerCharacter>(DetectingActor) && bIsDetected)
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
