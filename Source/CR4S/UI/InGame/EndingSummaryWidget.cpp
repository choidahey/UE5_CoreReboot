#include "UI/InGame/EndingSummaryWidget.h"
#include "Game/GameMode/C4SurvivalGameMode.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"

void UEndingSummaryWidget::NativeConstruct()
{
    PlayAnimation(FadeIn);
    SetEndingSummaryText();
    GetWorld()->GetTimerManager().SetTimer(
        ScrollTimerHandle, this, &UEndingSummaryWidget::ScrollEndingSummary, 0.01f, true);
}

void UEndingSummaryWidget::PlayEndingSequence()
{
    //
}

void UEndingSummaryWidget::SetEndingSummaryText()
{
    FString FilePath = FPaths::ProjectContentDir() + TEXT("CR4S/Credits.txt");
    FString FileData;

    if (FFileHelper::LoadFileToString(FileData, *FilePath))
    {
        UE_LOG(LogTemp, Log, TEXT("Loaded: %s"), *FileData);
        EndingSummaryText->SetText(FText::FromString(FileData));
    }
}

void UEndingSummaryWidget::ScrollEndingSummary()
{
    if (!EndingSummaryScrollBox) return;

    CurrentOffset += ScrollSpeed * 0.01f;

    EndingSummaryScrollBox->SetScrollOffset(CurrentOffset);

    float MaxOffset = EndingSummaryScrollBox->GetScrollOffsetOfEnd();
    if (CurrentOffset >= MaxOffset)
    {
        GetWorld()->GetTimerManager().ClearTimer(ScrollTimerHandle);
        OnEndingSequenceFinished();
    }
}

void UEndingSummaryWidget::OnEndingSequenceFinished()
{
    AC4SurvivalGameMode* GM = Cast<AC4SurvivalGameMode>(UGameplayStatics::GetGameMode(this));
    if (GM)
    {
        GM->ReturnToMenu();
    }
}
