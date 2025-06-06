#include "UI/Common/ButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CR4S_Button)
	{
		CR4S_Button->OnHovered.AddDynamic(this, &UButtonWidget::OnButtonHovered);
		CR4S_Button->OnClicked.AddDynamic(this, &UButtonWidget::OnButtonClicked);
	}
}

void UButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (ButtonText)
	{
		ButtonText->SetText(ButtonLabel);
	}
}

void UButtonWidget::OnButtonHovered()
{
	if (ButtonHoverSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ButtonHoverSound, 1.0f, 1.0f, HoverSoundStartTime);
	}
}

void UButtonWidget::OnButtonClicked()
{
	if (ButtonClickSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ButtonClickSound);
	}
}