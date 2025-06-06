#include "UI/InGame/CharacterEnvironmentStatusWidget.h"
#include "Game/System/EnvironmentManager.h"
#include "Character/Components/EnvironmentalStatusComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "CR4S.h"

void UCharacterEnvironmentStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();


	EnvironmentManager = Cast<AEnvironmentManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnvironmentManager::StaticClass()));
}

void UCharacterEnvironmentStatusWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsBoundToEnvironment && TryBindToPlayerCharacter())
	{
		bIsBoundToEnvironment = true;
		UE_LOG(LogEnvironment, Warning, TEXT("Environment Status Widget Binded To Player Character"));
	}
}


bool UCharacterEnvironmentStatusWidget::TryBindToPlayerCharacter()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return false;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return false;

	UEnvironmentalStatusComponent* EnvComp = Pawn->FindComponentByClass<UEnvironmentalStatusComponent>();
	if (!EnvComp) return false;

	EnvComp->OnTemperatureChanged.AddDynamic(this, &UCharacterEnvironmentStatusWidget::OnTemperatureChanged);
	EnvComp->OnHumidityChanged.AddDynamic(this, &UCharacterEnvironmentStatusWidget::OnHumidityChanged);

	OnTemperatureChanged(EnvComp->GetCurrentTemperature());
	OnHumidityChanged(EnvComp->GetCurrentHumidity());

	MinTempText->SetText(FText::AsNumber(EnvComp->GetMinTemperature()));
	MaxTempText->SetText(FText::AsNumber(EnvComp->GetMaxTemperature()));
	MinHuText->SetText(FText::AsNumber(EnvComp->GetMinHumidity()));
	MaxHuText->SetText(FText::AsNumber(EnvComp->GetMaxHumidity()));

	return true;
}

void UCharacterEnvironmentStatusWidget::OnTemperatureChanged(float NewTemperature)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	UEnvironmentalStatusComponent* EnvComp = Pawn->FindComponentByClass<UEnvironmentalStatusComponent>();
	if (EnvComp)
	{
		float NormalizedTemperature = (NewTemperature + 20.0f) / (30.0f + 20.0f);
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

		int32 RoundedTemp = FMath::RoundToInt(EnvComp->GetCurrentTemperature());
		CurrentTempText->SetText(FText::AsNumber(RoundedTemp));
	}
}

void UCharacterEnvironmentStatusWidget::OnHumidityChanged(float NewHumidity)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	UEnvironmentalStatusComponent* EnvComp = Pawn->FindComponentByClass<UEnvironmentalStatusComponent>();
	if (EnvComp)
	{
		HumidityProgressBar->SetPercent(NewHumidity / 100.0f);

		int32 RoundedHu = FMath::RoundToInt(EnvComp->GetCurrentHumidity());
		CurrentHuText->SetText(FText::AsNumber(RoundedHu));
	}
}