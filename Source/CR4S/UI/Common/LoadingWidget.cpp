#include "UI/Common/LoadingWidget.h"
#include "Components/Border.h"

void ULoadingWidget::HideImage()
{
	LoadingBorder->SetBrushColor(FLinearColor::Black);
}
