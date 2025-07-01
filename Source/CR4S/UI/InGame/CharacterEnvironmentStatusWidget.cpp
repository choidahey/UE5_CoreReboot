#include "UI/InGame/CharacterEnvironmentStatusWidget.h"
#include "Game/System/EnvironmentManager.h"
#include "Character/Components/EnvironmentalStatusComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "CR4S.h"
#include "Character/Components/BaseStatusComponent.h"
#include "GameFramework/Character.h"

void UCharacterEnvironmentStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();


	EnvironmentManager = Cast<AEnvironmentManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnvironmentManager::StaticClass()));
}

void UCharacterEnvironmentStatusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// if (!bIsBoundToEnvironment && TryBindToPlayerCharacter())
	// {
	// 	bIsBoundToEnvironment = true;
	// 	UE_LOG(LogEnvironment, Warning, TEXT("Environment Status Widget Binded To Player Character"));
	// }
}


// bool UCharacterEnvironmentStatusWidget::InitializeWidget(ACharacter* NewCharacter)
// {
// 	if (!CR4S_ENSURE(LogHong1,NewCharacter)) return false;
//
// 	CurrentCharacter=NewCharacter;
// 	UEnvironmentalStatusComponent* EnvComp = NewCharacter->FindComponentByClass<UEnvironmentalStatusComponent>();
// 	if (!CR4S_ENSURE(LogHong1,EnvComp)) return false;
//
// 	OnTemperatureChanged(EnvComp->GetCurrentTemperature());
// 	OnHumidityChanged(EnvComp->GetCurrentHumidity());
//
// 	UBaseStatusComponent* StatusComp=NewCharacter->FindComponentByClass<UBaseStatusComponent>();
// 	if (!CR4S_ENSURE(LogHong1,StatusComp)) return false;
// 	
// 	MinTempText->SetText(FText::AsNumber(StatusComp->GetColdThreshold()));
// 	MaxTempText->SetText(FText::AsNumber(StatusComp->GetHeatThreshold()));
// 	MinHuText->SetText(FText::AsNumber(0));
// 	MaxHuText->SetText(FText::AsNumber(StatusComp->GetHumidityThreshold()));
// 	
// 	return true;
// }

void UCharacterEnvironmentStatusWidget::InitializeWidget(UBaseStatusComponent* InStatus)
{
	CachedStatusComp = InStatus;
	InStatus->OnColdThresholdChanged.AddDynamic(this,&UCharacterEnvironmentStatusWidget::UpdateColdThreshold);
	InStatus->OnHeatThresholdChanged.AddDynamic(this,&UCharacterEnvironmentStatusWidget::UpdateHeatThreshold);
	InStatus->OnHumidityThresholdChanged.AddDynamic(this,&UCharacterEnvironmentStatusWidget::UpdateHumidityThreshold);

	MinTempText->SetText(FText::AsNumber(CachedStatusComp->GetColdThreshold()));
	MaxTempText->SetText(FText::AsNumber(CachedStatusComp->GetHeatThreshold()));
	MaxHuText->SetText(FText::AsNumber(CachedStatusComp->GetHumidityThreshold()));
}

void UCharacterEnvironmentStatusWidget::InitializeWidget(UEnvironmentalStatusComponent* InEnvStatus)
{
	CachedEnvironmentalStatusComp = InEnvStatus;
	InEnvStatus->OnTemperatureChanged.AddDynamic(this, &UCharacterEnvironmentStatusWidget::OnTemperatureChanged);
	InEnvStatus->OnHumidityChanged.AddDynamic(this, &UCharacterEnvironmentStatusWidget::OnHumidityChanged);

	OnTemperatureChanged(InEnvStatus->GetCurrentTemperature());
	OnHumidityChanged(InEnvStatus->GetCurrentHumidity());
}

void UCharacterEnvironmentStatusWidget::ClearBindingsToStatusComp()
{
	if (CachedStatusComp.IsValid())
	{
		CachedStatusComp->OnColdThresholdChanged.RemoveAll(this);
		CachedStatusComp->OnHeatThresholdChanged.RemoveAll(this);
		CachedStatusComp->OnHumidityThresholdChanged.RemoveAll(this);
		CachedStatusComp = nullptr;
	}
}

void UCharacterEnvironmentStatusWidget::ClearBindingsToEnvStatusComp()
{
	if (CachedEnvironmentalStatusComp.IsValid())
	{
		CachedEnvironmentalStatusComp->OnTemperatureChanged.RemoveAll(this);
		CachedEnvironmentalStatusComp->OnHumidityChanged.RemoveAll(this);
		CachedEnvironmentalStatusComp = nullptr;
	}
}

void UCharacterEnvironmentStatusWidget::OnTemperatureChanged(float NewTemperature)
{
	if (CachedEnvironmentalStatusComp.IsValid() && CachedStatusComp.IsValid())
	{

		float HeatThreshold=CachedStatusComp->GetHeatThreshold();
		float ColdThreshold=CachedStatusComp->GetColdThreshold();

		if (FMath::IsNearlyEqual(HeatThreshold,ColdThreshold)) return;
		
		float NormalizedTemperature = FMath::Clamp((NewTemperature -ColdThreshold) / (HeatThreshold - ColdThreshold),0,1);
		TemperatureProgressBar->SetPercent(NormalizedTemperature);

		FLinearColor ColdColor = FLinearColor(0.0f, 0.75f, 1.0f);
		FLinearColor MidColor = FLinearColor(1.0f, 1.0f, 0.4f);
		FLinearColor HotColor = FLinearColor(1.0f, 0.27f, 0.0f);

		FLinearColor InterpolatedColor;

		if (NormalizedTemperature < 0.5f)
		{
			float T = NormalizedTemperature / 0.5f;
			InterpolatedColor = FLinearColor::LerpUsingHSV(ColdColor, MidColor, T);
		}
		else
		{
			float T = (NormalizedTemperature - 0.5f) / 0.5f;
			InterpolatedColor = FLinearColor::LerpUsingHSV(MidColor, HotColor, T);
		}

		TemperatureProgressBar->SetFillColorAndOpacity(InterpolatedColor);

		int32 RoundedTemp = FMath::RoundToInt(CachedEnvironmentalStatusComp->GetCurrentTemperature());
		CurrentTempText->SetText(FText::AsNumber(RoundedTemp));
	}
}

void UCharacterEnvironmentStatusWidget::OnHumidityChanged(float NewHumidity)
{
	if (!CR4S_ENSURE(LogHong1,CachedEnvironmentalStatusComp.IsValid() && CachedStatusComp.IsValid())) return;

	const float HumidityThreshold = CachedStatusComp->GetHumidityThreshold();
	HumidityProgressBar->SetPercent(NewHumidity / HumidityThreshold);

	int32 RoundedHu = FMath::RoundToInt(NewHumidity);
	CurrentHuText->SetText(FText::AsNumber(RoundedHu));
}

void UCharacterEnvironmentStatusWidget::UpdateColdThreshold(float NewThreshold)
{
	MinTempText->SetText(FText::AsNumber(NewThreshold));
	if (CachedEnvironmentalStatusComp.IsValid())
	{
		OnTemperatureChanged(CachedEnvironmentalStatusComp->GetCurrentTemperature());
	}
}

void UCharacterEnvironmentStatusWidget::UpdateHeatThreshold(float NewThreshold)
{
	MaxTempText->SetText(FText::AsNumber(NewThreshold));
	if (CachedEnvironmentalStatusComp.IsValid())
	{
		OnTemperatureChanged(CachedEnvironmentalStatusComp->GetCurrentTemperature());
	}
}

void UCharacterEnvironmentStatusWidget::UpdateHumidityThreshold(float NewThreshold)
{
	MaxHuText->SetText(FText::AsNumber(NewThreshold));
	if (CachedEnvironmentalStatusComp.IsValid())
	{
		OnHumidityChanged(CachedEnvironmentalStatusComp->GetCurrentHumidity());
	}
}

void UCharacterEnvironmentStatusWidget::UpdateTemperatureProgressBar()
{
	if (!CR4S_ENSURE(LogHong1,CachedEnvironmentalStatusComp.IsValid())) return;

	OnTemperatureChanged(CachedEnvironmentalStatusComp->GetCurrentTemperature());
}

void UCharacterEnvironmentStatusWidget::UpdateHumidityProgressBar()
{
    if (!CR4S_ENSURE(LogHong1,CachedEnvironmentalStatusComp.IsValid())) return;
    
    OnHumidityChanged(CachedEnvironmentalStatusComp->GetCurrentHumidity());
}
