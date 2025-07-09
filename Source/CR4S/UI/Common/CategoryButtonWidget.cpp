#include "UI/Common/CategoryButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Game/System/AudioManager.h"

void UCategoryButtonWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (CategoryText)
    {
        CategoryText->SetText(CategoryLabel);
    }

    if (CategoryButton && ButtonTexture)
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

        CategoryButton->SetStyle(Style);
    }
}

void UCategoryButtonWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CategoryButton)
    {
        CategoryButton->OnClicked.AddDynamic(this, &UCategoryButtonWidget::OnButtonClicked);
        CategoryButton->OnHovered.AddDynamic(this, &UCategoryButtonWidget::OnButtonHovered);
        CategoryButton->OnUnhovered.AddDynamic(this, &UCategoryButtonWidget::OnButtonUnHovered);
    }
}

void UCategoryButtonWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (CategoryButton)
    {
        FVector2D Current = CategoryButton->RenderTransform.Translation;
        FVector2D New = FMath::Vector2DInterpTo(Current, TargetOffset, InDeltaTime, LerpSpeed);
        CategoryButton->SetRenderTranslation(New);
    }
}


void UCategoryButtonWidget::OnButtonClicked()
{
    UAudioManager* AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>();
    AudioManager->PlayUISound(ClickSound);

    OnCategoryClicked.Broadcast(CategoryLabel);
}

void UCategoryButtonWidget::OnButtonHovered()
{
    TargetOffset = FVector2D(HoverOffsetAmount, 0.f);

    UAudioManager* AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>();
    AudioManager->PlayUISound(HoverSound);
}

void UCategoryButtonWidget::OnButtonUnHovered()
{
    TargetOffset = FVector2D::ZeroVector;
}

void UCategoryButtonWidget::SetCategoryLabel(const FText& InLabel)
{
    CategoryLabel = InLabel;

    if (CategoryText)
    {
        CategoryText->SetText(CategoryLabel);
    }
}