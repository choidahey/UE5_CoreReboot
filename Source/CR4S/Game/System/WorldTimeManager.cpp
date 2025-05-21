#include "Game/System/WorldTimeManager.h"
#include "Game/System/SeasonManager.h"
#include "Game/System/EnvironmentManager.h"
#include "UI/InGame/SurvivalHUD.h"
#include "Kismet/GameplayStatics.h"

bool UWorldTimeManager::ShouldCreateSubsystem(UObject* Outer) const
{
    UWorld* World = Cast<UWorld>(Outer);
    if (World && World->GetName() == TEXT("SurvivalLevel"))
    {
		UE_LOG(LogTemp, Warning, TEXT("WorldTimeManager: ShouldCreateSubsystem called for SurvivalLevel"));
		return true;  // Creates this subsystem only in the SurvivalLevel world
    }

	UE_LOG(LogTemp, Warning, TEXT("WorldTimeManager: ShouldCreateSubsystem called for other world"));
    return false;
}

void UWorldTimeManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Warning, TEXT("WorldTimeManager: Initialize called"));

	LoadTimeData();

    FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &UWorldTimeManager::OnPostWorldInit);
	FWorldDelegates::OnWorldCleanup.AddUObject(this, &UWorldTimeManager::OnWorldCleanup);
}

void UWorldTimeManager::LoadTimeData()
{
	// if Saved Data Exists, Load Time Data
}

void UWorldTimeManager::OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS)
{
	if (World == GetWorld())
	{
		if (!World->GetTimerManager().IsTimerActive(TimeUpdateHandle))
		{
			World->GetTimerManager().SetTimer(TimeUpdateHandle, this, &UWorldTimeManager::UpdateTime, 1.0f / WorldTimeMultiplier, true);
		}
	}

	EnvironmentManager = Cast<AEnvironmentManager>(UGameplayStatics::GetActorOfClass(World, AEnvironmentManager::StaticClass()));
	if (!EnvironmentManager)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldTimeManager: EnvironmentManager not found"));
	}
}

void UWorldTimeManager::UpdateTime()
{
	TotalPlayTime += 1;
	CurrentTimeData.Second += 1;
	if (CurrentTimeData.Second >= 60)
	{
		CurrentTimeData.Second = 0;
		CurrentTimeData.Minute += 1;
		if (CurrentTimeData.Minute >= DayLength)
		{
			CurrentTimeData.Minute = 0;
			CurrentTimeData.Day += 1;
			if (CurrentTimeData.Day > SeasonLength)
			{
				CurrentTimeData.Day = 1;
				USeasonManager* SeasonManager = GetWorld()->GetSubsystem<USeasonManager>();
				if (SeasonManager)
				{
					SeasonManager->ChangeToNextSeason();
					UE_LOG(LogTemp, Warning, TEXT("WorldTimeManager: Season changed"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("WorldTimeManager: SeasonManager not found"));
				}
			}
		}
	}

	UpdateTimeWidget();
	OnWorldTimeUpdated.Broadcast(TotalPlayTime);
	AdvanceSkyTime(CurrentTimeData.Minute, CurrentTimeData.Second);
}

void UWorldTimeManager::PauseTime()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().PauseTimer(TimeUpdateHandle);
		UE_LOG(LogTemp, Warning, TEXT("WorldTimeManager: Time paused"));
	}
}

void UWorldTimeManager::ResumeTime()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().UnPauseTimer(TimeUpdateHandle);
		UE_LOG(LogTemp, Warning, TEXT("WorldTimeManager: Time resumed"));
	}
}

void UWorldTimeManager::OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	// Cleanup logic if needed
	UE_LOG(LogTemp, Warning, TEXT("WorldTimeManager: OnWorldCleanup called"));
	World->GetTimerManager().ClearTimer(TimeUpdateHandle);
}

void UWorldTimeManager::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Warning, TEXT("WorldTimeManager: Deinitialize called"));
}

void UWorldTimeManager::ModifyTime(int32 Day, int32 Minute)
{
	TotalPlayTime = TotalPlayTime + Day * DayLength * 60 + Minute * 60; // Convert to seconds
	UE_LOG(LogTemp, Warning, TEXT("WorldTimeManager: Total Play Time modified to %lld seconds"), TotalPlayTime);

	CurrentTimeData.Day += Day;
	CurrentTimeData.Minute += Minute;

	for (CurrentTimeData.Minute; CurrentTimeData.Minute >= DayLength; CurrentTimeData.Minute -= DayLength)
	{
		CurrentTimeData.Day += 1;
		if (CurrentTimeData.Day > SeasonLength)
		{
			CurrentTimeData.Day = 1;

			USeasonManager* SeasonManager = GetWorld()->GetSubsystem<USeasonManager>();
			if (SeasonManager)
			{
				SeasonManager->ChangeToNextSeason();
				UE_LOG(LogTemp, Warning, TEXT("WorldTimeManager: Season changed"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("WorldTimeManager: SeasonManager not found"));
			}
		}
	}

	for (CurrentTimeData.Day; CurrentTimeData.Day > SeasonLength; CurrentTimeData.Day -= SeasonLength)
	{
		USeasonManager* SeasonManager = GetWorld()->GetSubsystem<USeasonManager>();
		if (SeasonManager)
		{
			SeasonManager->ChangeToNextSeason();
			UE_LOG(LogTemp, Warning, TEXT("WorldTimeManager: Season changed"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("WorldTimeManager: SeasonManager not found"));
		}
	}


	UpdateTimeWidget();
	OnWorldTimeUpdated.Broadcast(TotalPlayTime);
	AdvanceSkyTime(CurrentTimeData.Minute, CurrentTimeData.Second);
}

void UWorldTimeManager::UpdateTimeWidget()
{
	if (!GetWorld()) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	ASurvivalHUD* InGameHUD = Cast<ASurvivalHUD>(PC->GetHUD());
	if (!InGameHUD) return;

	InGameHUD->SetTime(GetCurrentTimeData());
}

void UWorldTimeManager::AdvanceSkyTime(int32 Min, int32 Sec)
{
	if (!EnvironmentManager) {return;}

	const int32 TotalSecondsInDay = DayLength * 60;
	const int32 CurrentSeconds = Min * 60 + Sec;

	const float TimeRatio = static_cast<float>(CurrentSeconds) / static_cast<float>(TotalSecondsInDay);
	const int32 TimeOfDay = FMath::Clamp(static_cast<int32>(TimeRatio * 2400.0f), 0, 2400);

	EnvironmentManager->SetSkyTime(TimeOfDay);
}