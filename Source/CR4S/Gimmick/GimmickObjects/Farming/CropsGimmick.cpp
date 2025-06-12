#include "CropsGimmick.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Game/System/WorldTimeManager.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Gimmick/Data/GimmickData.h"

ACropsGimmick::ACropsGimmick()
	: HarvestText(FText::FromString(TEXT("수확 하기"))),
	  DetectingActor(nullptr),
	  bIsDetected(false),
	  bIsHarvestable(false),
	  GrowthTimeMinutes(0),
	  ElapsedSeconds(0),
	  TotalGrowthSeconds(0),
	  MaxStageCount(0),
	  CurrentStage(0),
	  CurrentGrowthPercent(0.f)
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));

	GimmickMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GimmickMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GimmickMeshComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
}

void ACropsGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
	{
		InteractableComponent->OnDetectionStateChanged.AddUniqueDynamic(this, &ThisClass::OnDetectionStateChanged);
		InteractableComponent->OnTryInteract.AddUniqueDynamic(this, &ThisClass::OnGimmickInteracted);

		DefaultInteractionText = InteractableComponent->GetInteractionText();
	}

	const FCropsGimmickData* CropsGimmickData = nullptr;
	if (const FGimmickInfoData* GimmickInfoData = GetGimmickInfoData())
	{
		const UDataTable* DataTable = GimmickInfoData->DetailData.DataTable;
		if (IsValid(DataTable))
		{
			const FName RowName = GimmickInfoData->DetailData.RowName;
			CropsGimmickData = DataTable->FindRow<FCropsGimmickData>(RowName, TEXT("CropsGimmickData"));
		}
	}

	if (CR4S_VALIDATE(LogGimmick, CropsGimmickData))
	{
		CropsMeshes = CropsGimmickData->CropsMeshes;

		if (CR4S_VALIDATE(LogGimmick, CropsMeshes.Num() > 0))
		{
			if (!bIsHarvestable)
			{
				if (IsValid(CropsMeshes[0]))
				{
					GimmickMeshComponent->SetStaticMesh(CropsMeshes[0]);

					GrowthTimeMinutes = CropsGimmickData->GrowthRate;
					ElapsedSeconds = 0;
					CurrentStage = 0;
					TotalGrowthSeconds = GrowthTimeMinutes * 60;
					MaxStageCount = CropsMeshes.Num() - 1;
					StageDuration = TotalGrowthSeconds / (MaxStageCount);
					CurrentGrowthPercent = 0.f;

					BindDelegate();
				}
			}
			else
			{
				if (IsValid(CropsMeshes.Last()))
				{
					GimmickMeshComponent->SetStaticMesh(CropsMeshes.Last());
					CurrentGrowthPercent = 100.f;
				}
			}
		}
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
			+ FString::Printf(TEXT(" %.1f%%"), CurrentGrowthPercent);

		FText Text = FText::FromString(InteractionString);

		if (bIsHarvestable)
		{
			const FString NewInteractionString = FString::Printf(TEXT("%s\n%s"),
			                                                     *HarvestText.ToString(), *InteractionString);
			Text = FText::FromString(NewInteractionString);
		}

		InteractableComponent->SetInteractionText(Text);
	}
}

void ACropsGimmick::Harvest(const AActor* Interactor)
{
	if (OnHarvest.IsBound())
	{
		OnHarvest.Broadcast();
	}

	GetResources(Interactor);

	GimmickDestroy();
}

void ACropsGimmick::Grow(int64 PlayTime)
{
	if (CropsMeshes.Num() < 2 || TotalGrowthSeconds <= 0.f)
	{
		return;
	}

	if (FMath::RandRange(1, 100) <= 70)
	{
		ElapsedSeconds += 1;
	}

	CurrentGrowthPercent = FMath::Clamp(ElapsedSeconds * 100.0f / TotalGrowthSeconds, 0.0f, 200.0f);

	if (OnGrow.IsBound())
	{
		OnGrow.Broadcast(CurrentGrowthPercent);
	}

	if (CurrentGrowthPercent == 200.f)
	{
		if (OnCropComposted.IsBound())
		{
			OnCropComposted.Broadcast();
		}
		Destroy();
		return;
	}

	if (CurrentGrowthPercent > 100.f)
	{
		return;
	}

	UpdateGrowthStage();
}

void ACropsGimmick::UpdateGrowthStage()
{
	const int32 NewStage = FMath::Clamp(
		FMath::FloorToInt(ElapsedSeconds / StageDuration),
		0,
		MaxStageCount
	);

	if (NewStage != CurrentStage)
	{
		CurrentStage = NewStage;
		if (CropsMeshes.IsValidIndex(CurrentStage) && CropsMeshes[CurrentStage])
		{
			GimmickMeshComponent->SetStaticMesh(CropsMeshes[CurrentStage]);

			if (CurrentStage == MaxStageCount)
			{
				bIsHarvestable = true;
			}
		}
	}
}

void ACropsGimmick::BindDelegate()
{
	UWorldTimeManager* WorldTimeManager = GetWorld()->GetSubsystem<UWorldTimeManager>();
	if (IsValid(WorldTimeManager))
	{
		WorldTimeManager->OnWorldTimeUpdated.AddUniqueDynamic(this, &ThisClass::Grow);
	}
}

void ACropsGimmick::UnBindDelegate()
{
	UWorldTimeManager* WorldTimeManager = GetWorld()->GetSubsystem<UWorldTimeManager>();
	if (IsValid(WorldTimeManager) && WorldTimeManager->OnWorldTimeUpdated.IsAlreadyBound(this, &ThisClass::Grow))
	{
		WorldTimeManager->OnWorldTimeUpdated.RemoveDynamic(this, &ThisClass::Grow);
	}
}
