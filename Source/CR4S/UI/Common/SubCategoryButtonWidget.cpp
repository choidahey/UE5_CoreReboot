#include "UI/Common/SubCategoryButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Game/System/AudioManager.h"

void USubCategoryButtonWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (SubCategoryText)
    {
        SubCategoryText->SetText(SubCategoryLabel);
    }

    if (SubCategoryButton && ButtonTexture)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(ButtonTexture);
        //Brush.ImageSize = FVector2D(64, 64);

        FButtonStyle Style;
        Brush.TintColor = FSlateColor(NormalTint);
        Style.SetNormal(Brush);

        Brush.TintColor = FSlateColor(HoveredTint);
        Style.SetHovered(Brush);

        Brush.TintColor = FSlateColor(PressedTint);
        Style.SetPressed(Brush);

        SubCategoryButton->SetStyle(Style);
    }
}

void USubCategoryButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SubCategoryButton)
	{
		SubCategoryButton->OnClicked.AddDynamic(this, &USubCategoryButtonWidget::OnButtonClicked);
		SubCategoryButton->OnHovered.AddDynamic(this, &USubCategoryButtonWidget::OnButtonHovered);
	}
}

void USubCategoryButtonWidget::OnButtonClicked()
{
	UAudioManager* AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>();
    AudioManager->PlayUISound(ClickSound);

    OnSubCategoryClicked.Broadcast(SubCategoryLabel);
}

void USubCategoryButtonWidget::OnButtonHovered()
{
    UAudioManager* AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>();
    AudioManager->PlayUISound(HoverSound);
}

void USubCategoryButtonWidget::SetSubCategoryLabel(const FText& InLabel)
{
    SubCategoryLabel = InLabel;

    if (SubCategoryText)
    {
        SubCategoryText->SetText(SubCategoryLabel);
    }
}