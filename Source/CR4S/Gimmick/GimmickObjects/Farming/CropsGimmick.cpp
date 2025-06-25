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
			if (const FCropsGimmickData* Data
				= DataTable->FindRow<FCropsGimmickData>(RowName, TEXT("CropsGimmickData")))
			{
				CropsGimmickGrowthData.CropsGimmickData = *Data;
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
			+ FString::Printf(TEXT(" %.1f%%"), CropsGimmickGrowthData.CurrentGrowthPercent);

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
	CropsMeshes = CropsGimmickGrowthData.CropsGimmickData.CropsMeshes;

	if (CR4S_VALIDATE(LogGimmick, CropsMeshes.Num() > 0))
	{
		if (!bIsHarvestable)
		{
			if (IsValid(CropsMeshes[0]))
			{
				GimmickMeshComponent->SetStaticMesh(CropsMeshes[0]);

				CropsGimmickGrowthData.GrowthTimeMinutes = CropsGimmickGrowthData.CropsGimmickData.GrowthRate;
				CropsGimmickGrowthData.ElapsedSeconds = 0;
				CropsGimmickGrowthData.CurrentStage = 0;
				CropsGimmickGrowthData.TotalGrowthSeconds = CropsGimmickGrowthData.GrowthTimeMinutes * 60;
				CropsGimmickGrowthData.MaxStageCount = CropsMeshes.Num() - 1;
				CropsGimmickGrowthData.StageDuration = CropsGimmickGrowthData.TotalGrowthSeconds / CropsGimmickGrowthData.MaxStageCount;
				CropsGimmickGrowthData.CurrentGrowthPercent = 0.f;

				BindDelegate();
			}
		}
		else
		{
			if (IsValid(CropsMeshes.Last()))
			{
				GimmickMeshComponent->SetStaticMesh(CropsMeshes.Last());
				CropsGimmickGrowthData.CurrentGrowthPercent = 100.f;
			}
		}
	}
}

void ACropsGimmick::Grow(const int64 NewPlayTime)
{
	if (CropsMeshes.Num() < 2 || CropsGimmickGrowthData.TotalGrowthSeconds <= 0.f)
	{
		return;
	}

	if (CropsGimmickGrowthData.PrevPlayTime < 0)
	{
		CropsGimmickGrowthData.PrevPlayTime = NewPlayTime;
		return;
	}

	const int64 DeltaInt = NewPlayTime - CropsGimmickGrowthData.PrevPlayTime;
	CropsGimmickGrowthData.PrevPlayTime = NewPlayTime;

	if (DeltaInt <= 0)
	{
		return;
	}

	for (int64 Time = 0; Time < DeltaInt; Time++)
	{
		if (FMath::RandRange(1, 100) <= 70)
		{
			CropsGimmickGrowthData.ElapsedSeconds += 1;
		}

		CropsGimmickGrowthData.CurrentGrowthPercent
			= FMath::Clamp(CropsGimmickGrowthData.ElapsedSeconds * 100.0f / CropsGimmickGrowthData.TotalGrowthSeconds,
			               0.0f,
			               200.0f);

		if (OnGrow.IsBound())
		{
			OnGrow.Broadcast(CropsGimmickGrowthData.CurrentGrowthPercent);
		}

		if (CropsGimmickGrowthData.CurrentGrowthPercent == 200.f)
		{
			if (OnCropComposted.IsBound())
			{
				OnCropComposted.Broadcast();
			}
			Destroy();
			continue;
		}

		if (CropsGimmickGrowthData.CurrentGrowthPercent > 100.f)
		{
			continue;
		}

		UpdateGrowthStage();
	}
}

void ACropsGimmick::UpdateGrowthStage()
{
	const int32 NewStage = FMath::Clamp(
		FMath::FloorToInt(CropsGimmickGrowthData.ElapsedSeconds / CropsGimmickGrowthData.StageDuration),
		0,
		CropsGimmickGrowthData.MaxStageCount
	);

	if (NewStage != CropsGimmickGrowthData.CurrentStage)
	{
		CropsGimmickGrowthData.CurrentStage = NewStage;
		if (CropsMeshes.IsValidIndex(CropsGimmickGrowthData.CurrentStage) && CropsMeshes[CropsGimmickGrowthData.CurrentStage])
		{
			GimmickMeshComponent->SetStaticMesh(CropsMeshes[CropsGimmickGrowthData.CurrentStage]);

			if (CropsGimmickGrowthData.CurrentStage == CropsGimmickGrowthData.MaxStageCount)
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

void ACropsGimmick::LoadPlantedCropsGimmick(const FCropsGimmickGrowthData& NewCropsGimmickData)
{
	CropsGimmickGrowthData = NewCropsGimmickData;
	CropsGimmickGrowthData.PrevPlayTime = -1;
	CropsGimmickGrowthData.CurrentStage = 0;

	UpdateGrowthStage();
}
