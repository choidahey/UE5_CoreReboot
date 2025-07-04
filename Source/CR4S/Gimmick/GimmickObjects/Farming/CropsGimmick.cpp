#include "CropsGimmick.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Character/Components/EnvironmentalStatusComponent.h"
#include "Game/SaveGame/GimmickSaveGame.h"
#include "Game/System/AudioManager.h"
#include "Game/System/EnvironmentManager.h"
#include "Game/System/WorldTimeManager.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Gimmick/Data/GimmickData.h"
#include "Kismet/GameplayStatics.h"

ACropsGimmick::ACropsGimmick()
	: HarvestText(FText::FromString(TEXT("수확 하기"))),
	  DetectingActor(nullptr),
	  bIsDetected(false),
	  bIsHarvestable(true),
	  bIsPlanted(false),
	  HeatSlowdownMultiplier(1.f),
	  HumiditySlowdownMultiplier(1.f),
	  AccelerationMultiplier(1.f)
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));

	InitCollisionSetting();

	EnvironmentalStatus = CreateDefaultSubobject<UEnvironmentalStatusComponent>(TEXT("EnvironmentalStatus"));

	GimmickMeshComponent->SetCullDistance(15000);
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

	InitCollisionSetting();	

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
				
				HarvestSound = GimmickInfoData->InteractSound;
			}
		}
	}

	InitEnvironmentalStatus();
	
	InitGrowthState();

	AActor* FindActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnvironmentManager::StaticClass());
	EnvironmentManager = Cast<AEnvironmentManager>(FindActor);
}

FGimmickSaveGameData ACropsGimmick::GetGimmickSaveGameData_Implementation(bool& bSuccess)
{
	bSuccess = false;
	return bIsPlanted ? FGimmickSaveGameData() : Super::GetGimmickSaveGameData_Implementation(bSuccess);
}

void ACropsGimmick::InitCollisionSetting() const
{
	if (IsValid(GimmickMeshComponent))
	{
		GimmickMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		GimmickMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		GimmickMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		GimmickMeshComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
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
	PlaySFX(HarvestSound, GetActorLocation(), EConcurrencyType::Default);
	
	if (OnHarvest.IsBound())
	{
		OnHarvest.Broadcast();
	}

	GetResources(Interactor);

	OnSpawnableDestroyed.Broadcast(this);
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
				CropsGimmickGrowthData.StageDuration = CropsGimmickGrowthData.TotalGrowthSeconds /
					CropsGimmickGrowthData.MaxStageCount;
				CropsGimmickGrowthData.CurrentGrowthPercent = 0.f;

				BindDelegate();
				BindEnvStatusDelegate();
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

float ACropsGimmick::GetCalculatedSeconds()
{
	CheckAcceleration();
	
	return HeatSlowdownMultiplier
	       * HumiditySlowdownMultiplier
	       * AccelerationMultiplier;
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
			CropsGimmickGrowthData.ElapsedSeconds += GetCalculatedSeconds();
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
		if (CropsMeshes.IsValidIndex(CropsGimmickGrowthData.CurrentStage) && CropsMeshes[CropsGimmickGrowthData.
			CurrentStage])
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

void ACropsGimmick::HandleTemperatureBreach(int32 BreachCode)
{
	HeatSlowdownMultiplier = CropsGimmickGrowthData.CropsGimmickData.HeatSlowdownMultiplier;
}

void ACropsGimmick::HandleHumidityBreach(int32 BreachCode)
{
	HumiditySlowdownMultiplier = CropsGimmickGrowthData.CropsGimmickData.HumiditySlowdownMultiplier;
}

void ACropsGimmick::HandleTemperatureNormalized()
{
	HeatSlowdownMultiplier = 1.f;
}

void ACropsGimmick::HandleHumidityNormalized()
{
	HumiditySlowdownMultiplier = 1.f;
}

void ACropsGimmick::BindEnvStatusDelegate()
{
	if (IsValid(EnvironmentalStatus))
	{
		EnvironmentalStatus->OnTemperatureBreach.AddUniqueDynamic(this, &ThisClass::HandleTemperatureBreach);
		EnvironmentalStatus->OnHumidityBreach.AddUniqueDynamic(this, &ThisClass::HandleHumidityBreach);
		EnvironmentalStatus->OnTemperatureNormalized.AddUniqueDynamic(this, &ThisClass::HandleTemperatureNormalized);
		EnvironmentalStatus->OnHumidityNormalized.AddUniqueDynamic(this, &ThisClass::HandleHumidityNormalized);
	}
}

void ACropsGimmick::CheckIsDay(bool& bSuccess, bool& bIsDay) const
{
	if (IsValid(EnvironmentManager))
	{
		bSuccess = true;
		const float CurrentTime = EnvironmentManager->GetCurrentTimeOfDay();
		const float DawnTime = EnvironmentManager->GetCurrentDawnTime();
		const float DuskTime = EnvironmentManager->GetCurrentDuskTime();

		bIsDay = CurrentTime >= DawnTime && CurrentTime <= DuskTime;
		return;
	}

	bSuccess = false;
}

void ACropsGimmick::CheckAcceleration()
{
	const FCropsGimmickData& CropsGimmickData = CropsGimmickGrowthData.CropsGimmickData;
	bool bCheckIsDaySuccess = false;
	bool bIsDay = false;
	CheckIsDay(bCheckIsDaySuccess, bIsDay);

	AccelerationMultiplier = 1.f;
	
	if (CropsGimmickData.bIsDay && bCheckIsDaySuccess && !bIsDay)
	{
		return;
	}

	if (CropsGimmickData.bIsNight && bCheckIsDaySuccess && bIsDay)
	{
		return;
	}

	if (IsValid(EnvironmentalStatus))
	{
		const float CurrentTemp = EnvironmentalStatus->GetCurrentTemperature();
		const float CurrentHum = EnvironmentalStatus->GetCurrentHumidity();

		if (CropsGimmickData.bUseHeatMaxValue && CropsGimmickData.HeatMaxValue > CurrentTemp)
		{
			return;
		}

		if (CropsGimmickData.bUseHeatMinValue && CropsGimmickData.HeatMinValue < CurrentTemp)
		{
			return;
		}

		if (CropsGimmickData.bUseHumidityMaxValue && CropsGimmickData.HumidityMaxValue > CurrentHum)
		{
			return;
		}

		if (CropsGimmickData.bUseHumidityMinValue && CropsGimmickData.HumidityMinValue < CurrentHum)
		{
			return;
		}
	}

	AccelerationMultiplier = CropsGimmickData.AccelerationMultiplier;
}

void ACropsGimmick::InitEnvironmentalStatus() const
{
	if (IsValid(EnvironmentalStatus))
	{
		EnvironmentalStatus->SetMaxTemperature(CropsGimmickGrowthData.CropsGimmickData.HeatMaxThreshold);
		EnvironmentalStatus->SetMinTemperature(CropsGimmickGrowthData.CropsGimmickData.HeatMinThreshold);
		EnvironmentalStatus->SetMaxHumidity(CropsGimmickGrowthData.CropsGimmickData.HumidityMaxThreshold);
		EnvironmentalStatus->SetMinHumidity(CropsGimmickGrowthData.CropsGimmickData.HumidityMinThreshold);
	}
}

void ACropsGimmick::LoadPlantedCropsGimmick(const FCropsGimmickGrowthData& NewCropsGimmickData)
{
	CropsGimmickGrowthData = NewCropsGimmickData;
	CropsGimmickGrowthData.PrevPlayTime = -1;
	CropsGimmickGrowthData.CurrentStage = 0;

	UpdateGrowthStage();
}
