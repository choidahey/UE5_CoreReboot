#include "AnimalInteractWidget.h"
#include "Components/Button.h"
#include "../BaseAnimal.h"

void UAnimalInteractWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(CaptureButton))
	{
		CaptureButton->OnClicked.AddDynamic(this, &UAnimalInteractWidget::OnClickedCapture);
	}

	if (IsValid(ButcherButton))
	{
		ButcherButton->OnClicked.AddDynamic(this, &UAnimalInteractWidget::OnClickedButcher);
	}
}

void UAnimalInteractWidget::InitByAnimalState(bool bIsStunned)
{
	if (IsValid(CaptureButton))
	{
		CaptureButton->SetIsEnabled(bIsStunned);
	}
}

void UAnimalInteractWidget::OnClickedCapture()
{
	if (IsValid(OwningAnimal))
	{
		OwningAnimal->Capture();
	}
}

void UAnimalInteractWidget::OnClickedButcher()
{
	if (IsValid(OwningAnimal))
	{
		OwningAnimal->Capture();
	}
}
