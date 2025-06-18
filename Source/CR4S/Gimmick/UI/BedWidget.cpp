#include "BedWidget.h"

#include "CR4S.h"
#include "Animation/WidgetAnimation.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "Components/Button.h"
#include "Game/System/EnvironmentManager.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InGame/SurvivalHUD.h"

void UBedWidget::InitWidget()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (IsValid(PlayerController))
	{
		if (IsValid(GEngine))
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);

			const FVector2D ScreenCenter = ViewportSize * 0.5f;
			PlayerController->SetMouseLocation(ScreenCenter.X, ScreenCenter.Y);
		}

		SurvivalHUD = Cast<ASurvivalHUD>(PlayerController->GetHUD());
		if (IsValid(SurvivalHUD))
		{
			SurvivalHUD->SetInputMode(ESurvivalInputMode::UIOnly, this, true);
		}
	}

	if (IsValid(SleepButton))
	{
		if (CanSleep())
		{
			SleepButton->SetIsEnabled(true);
			SleepButton->OnClicked.AddUniqueDynamic(this, &ThisClass::PlaySleepingAnimation);
		}
		else
		{
			SleepButton->SetIsEnabled(false);
		}
	}

	if (IsValid(CancelButton))
	{
		CancelButton->OnClicked.AddUniqueDynamic(this, &ThisClass::RemoveWidget);
	}

	if (IsValid(SleepingAnim))
	{
		FWidgetAnimationDynamicEvent Delegate;
		Delegate.BindDynamic(this, &ThisClass::HandleAnimationFinished);
		BindToAnimationFinished(SleepingAnim, Delegate);
	}
}

bool UBedWidget::CanSleep() const
{
	AActor* TargetActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnvironmentManager::StaticClass());
	AEnvironmentManager* EnvironmentManager = Cast<AEnvironmentManager>(TargetActor);
	if (CR4S_VALIDATE(LogGimmickUI, IsValid(EnvironmentManager)))
	{
		return EnvironmentManager->IsSleepTime();
	}

	return true;
}

void UBedWidget::PlaySleepingAnimation()
{
	if (IsValid(SleepingAnim))
	{
		if (IsValid(SurvivalHUD))
		{
			SurvivalHUD->SetInputMode(ESurvivalInputMode::UIOnly, this, false);
		}

		PlayAnimation(SleepingAnim, 0.f, 1, EUMGSequencePlayMode::Forward);

		bIsSleeping = true;
	}
}

void UBedWidget::PlayAwakingAnimation()
{
	if (IsValid(SleepingAnim))
	{
		bIsSleeping = false;
		PlayAnimation(SleepingAnim, 0.f, 1, EUMGSequencePlayMode::Reverse);
	}
}

void UBedWidget::HandleAnimationFinished()
{
	if (bIsSleeping)
	{
		CloseButtonWidgets();

		AActor* TargetActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEnvironmentManager::StaticClass());
		AEnvironmentManager* EnvironmentManager = Cast<AEnvironmentManager>(TargetActor);
		if (CR4S_VALIDATE(LogGimmickUI, IsValid(EnvironmentManager)))
		{
			const float CurrentTime = EnvironmentManager->GetCurrentTimeOfDay();
			const float TargetTime = EnvironmentManager->GetCurrentDawnTime() + AddTime;
			ModifyStat(TargetTime - CurrentTime);
			EnvironmentManager->AdvanceTimeOfDay(TargetTime);
		}

		PlayAwakingAnimation();
	}
	else
	{
		RemoveWidget();
	}
}

void UBedWidget::ModifyStat(const float SleepingTime) const
{
	CR4S_Log(LogGimmickUI, Warning, TEXT("SleepingTime: %.2f"), SleepingTime);
	
	APawn* Pawn = GetOwningPlayerPawn();
	if (IsValid(Pawn))
	{
		UPlayerCharacterStatusComponent* StatusComponent
			= Pawn->FindComponentByClass<UPlayerCharacterStatusComponent>();
		if (IsValid(StatusComponent))
		{
			const float MaxHP = StatusComponent->GetMaxHP();
			const float HealthRecoveryValue = CalculateHealthRecovery(SleepingTime) / 100.f;
			StatusComponent->AddCurrentHP(MaxHP * HealthRecoveryValue);

			CR4S_Log(LogGimmickUI, Warning, TEXT("HealthRecovery: %.2f"), HealthRecoveryValue);
			
			const float HungerReductionValue = CalculateHungerReduction(SleepingTime);
			StatusComponent->AddCurrentHunger(-HungerReductionValue);

			CR4S_Log(LogGimmickUI, Warning, TEXT("HungerReduction: %.2f"), HungerReductionValue);
		}
	}
}

float UBedWidget::CalculateHealthRecovery(const float SleepingTime) const
{
	const float Alpha = FMath::Clamp(SleepingTime / MaxSleepTime, 0.0f, 1.0f);
	return FMath::Lerp(10.0f, 50.0f, Alpha);
}

float UBedWidget::CalculateHungerReduction(const float SleepingTime) const
{
	const float Alpha = FMath::Clamp(SleepingTime / MaxSleepTime, 0.0f, 1.0f);
	return FMath::Lerp(10.0f, 50.0f, Alpha);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBedWidget::RemoveWidget()
{
	if (IsValid(SurvivalHUD))
	{
		SurvivalHUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
	}

	if (OnWidgetClose.IsBound())
	{
		OnWidgetClose.Broadcast();
	}

	RemoveFromParent();
}

void UBedWidget::CloseButtonWidgets() const
{
	if (IsValid(ButtonContainer))
	{
		ButtonContainer->SetVisibility(ESlateVisibility::Hidden);
	}
}
