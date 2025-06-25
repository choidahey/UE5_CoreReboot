#include "CropsGimmick.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Character/Components/EnvironmentalStatusComponent.h"
#include "Game/System/WorldTimeManager.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Gimmick/Data/GimmickData.h"

ACropsGimmick::ACropsGimmick()
	: HarvestText(FText::FromString(TEXT("수확 하기"))),
	  DetectingActor(nullptr),
	  bIsDetected(false),
	  bIsHarvestable(true),
	  bIsPlanted(false)
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));

	GimmickMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GimmickMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GimmickMeshComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);

	EnvironmentalStatus = CreateDefaultSubobject<UEnvironmentalStatusComponent>(TEXT("EnvironmentalStatus"));
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

	if (const FGimmickInfoData* GimmickInfoData = GetGimmickInfoData())
	{
		const UDataTable* DataTable = GimmickInfoData->DetailData.DataTable;
		if (IsValid(DataTable))
		{
			const FName RowName = GimmickInfoData->DetailData.RowName;
			if (const FCropsGimmickDetailData* Data
				= DataTable->FindRow<FCropsGimmickDetailData>(RowName, TEXT("CropsGimmickData")))
			{
				CropsGimmickData.CropsGimmickDetailData = *Data;
			}
		}
	}

	InitGrowthState();
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
			+ FString::Printf(TEXT(" %.1f%%"), CropsGimmickData.CurrentGrowthPercent);

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

void ACropsGimmick::OnPlant()
{
	bIsPlanted = true;
	bIsHarvestable = false;
	InitGrowthState();
}

void ACropsGimmick::InitGrowthState()
{
	CropsMeshes = CropsGimmickData.CropsGimmickDetailData.CropsMeshes;

	if (CR4S_VALIDATE(LogGimmick, CropsMeshes.Num() > 0))
	{
		if (!bIsHarvestable)
		{
			if (IsValid(CropsMeshes[0]))
			{
				GimmickMeshComponent->SetStaticMesh(CropsMeshes[0]);

				CropsGimmickData.GrowthTimeMinutes = CropsGimmickData.CropsGimmickDetailData.GrowthRate;
				CropsGimmickData.ElapsedSeconds = 0;
				CropsGimmickData.CurrentStage = 0;
				CropsGimmickData.TotalGrowthSeconds = CropsGimmickData.GrowthTimeMinutes * 60;
				CropsGimmickData.MaxStageCount = CropsMeshes.Num() - 1;
				CropsGimmickData.StageDuration = CropsGimmickData.TotalGrowthSeconds / CropsGimmickData.MaxStageCount;
				CropsGimmickData.CurrentGrowthPercent = 0.f;

				BindDelegate();
			}
		}
		else
		{
			if (IsValid(CropsMeshes.Last()))
			{
				GimmickMeshComponent->SetStaticMesh(CropsMeshes.Last());
				CropsGimmickData.CurrentGrowthPercent = 100.f;
			}
		}
	}
}

void ACropsGimmick::Grow(const int64 NewPlayTime)
{
	if (CropsMeshes.Num() < 2 || CropsGimmickData.TotalGrowthSeconds <= 0.f)
	{
		return;
	}

	if (CropsGimmickData.PrevPlayTime < 0)
	{
		CropsGimmickData.PrevPlayTime = NewPlayTime;
		return;
	}

	const int64 DeltaInt = NewPlayTime - CropsGimmickData.PrevPlayTime;
	CropsGimmickData.PrevPlayTime = NewPlayTime;

	if (DeltaInt <= 0)
	{
		return;
	}

	for (int64 Time = 0; Time < DeltaInt; Time++)
	{
		if (FMath::RandRange(1, 100) <= 70)
		{
			CropsGimmickData.ElapsedSeconds += 1;
		}

		CropsGimmickData.CurrentGrowthPercent
			= FMath::Clamp(CropsGimmickData.ElapsedSeconds * 100.0f / CropsGimmickData.TotalGrowthSeconds,
			               0.0f,
			               200.0f);

		if (OnGrow.IsBound())
		{
			OnGrow.Broadcast(CropsGimmickData.CurrentGrowthPercent);
		}

		if (CropsGimmickData.CurrentGrowthPercent == 200.f)
		{
			if (OnCropComposted.IsBound())
			{
				OnCropComposted.Broadcast();
			}
			Destroy();
			continue;
		}

		if (CropsGimmickData.CurrentGrowthPercent > 100.f)
		{
			continue;
		}

		UpdateGrowthStage();
	}
}

void ACropsGimmick::UpdateGrowthStage()
{
	const int32 NewStage = FMath::Clamp(
		FMath::FloorToInt(CropsGimmickData.ElapsedSeconds / CropsGimmickData.StageDuration),
		0,
		CropsGimmickData.MaxStageCount
	);

	if (NewStage != CropsGimmickData.CurrentStage)
	{
		CropsGimmickData.CurrentStage = NewStage;
		if (CropsMeshes.IsValidIndex(CropsGimmickData.CurrentStage) && CropsMeshes[CropsGimmickData.CurrentStage])
		{
			GimmickMeshComponent->SetStaticMesh(CropsMeshes[CropsGimmickData.CurrentStage]);

			if (CropsGimmickData.CurrentStage == CropsGimmickData.MaxStageCount)
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

void ACropsGimmick::LoadPlantedCropsGimmick(const FCropsGimmickData& NewCropsGimmickData)
{
	CropsGimmickData = NewCropsGimmickData;
	CropsGimmickData.PrevPlayTime = -1;
	CropsGimmickData.CurrentStage = 0;
}
