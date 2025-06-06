#include "BarnManagementWidget.h"
#include "Components/Button.h"
#include "Gimmick/GimmickObjects/Buildings/BarnGimmick.h"

void UBarnManagementWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(ExitButton))
	{
		ExitButton->OnClicked.AddDynamic(this, &UBarnManagementWidget::OnExitClicked);
	}
	if (IsValid(TestButton))
	{
		TestButton->OnClicked.AddDynamic(this, &UBarnManagementWidget::OnTestButtonClicked);
	}
	if (IsValid(TestButton2))
	{
		TestButton2->OnClicked.AddDynamic(this, &UBarnManagementWidget::OnTestButton2Clicked);
	}
}

void UBarnManagementWidget::OnExitClicked()
{
	RemoveFromParent();

	if (IsValid(OwningGimmick))
	{
		OwningGimmick->SetBarnUIInstance(nullptr);
	}
}

void UBarnManagementWidget::OnTestButtonClicked()
{
	if (IsValid(OwningGimmick))
	{
		OwningGimmick->SpawnAnimalByRowName("Wolf");
	}
}

void UBarnManagementWidget::OnTestButton2Clicked()
{
	if (IsValid(OwningGimmick))
	{
		OwningGimmick->SpawnAnimalByRowName("Deer");
	}
}
