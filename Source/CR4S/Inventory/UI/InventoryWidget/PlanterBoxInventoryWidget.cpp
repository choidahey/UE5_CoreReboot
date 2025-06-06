#include "PlanterBoxInventoryWidget.h"

#include "CR4S.h"
#include "Components/TextBlock.h"
#include "Gimmick/Data/GimmickData.h"
#include "Gimmick/GimmickObjects/Buildings/PlanterBoxGimmick.h"
#include "Gimmick/GimmickObjects/Farming/CropsGimmick.h"
#include "Inventory/Components/BaseInventoryComponent.h"

void UPlanterBoxInventoryWidget::ConnectInventoryComponent(UBaseInventoryComponent* NewInventoryComponent)
{
	Super::ConnectInventoryComponent(NewInventoryComponent);

	ConnectCropsGimmick();
}

void UPlanterBoxInventoryWidget::UpdateItemSlotWidget(const int32 SlotIndex, UBaseInventoryItem* Item)
{
	Super::UpdateItemSlotWidget(SlotIndex, Item);

	ConnectCropsGimmick();
}

void UPlanterBoxInventoryWidget::UnBoundWidgetDelegate()
{
	Super::UnBoundWidgetDelegate();

	if (IsValid(PlantedCropsGimmick))
	{
		if (PlantedCropsGimmick->OnGrow.IsAlreadyBound(this, &ThisClass::UpdatePlanterBoxInfo))
		{
			PlantedCropsGimmick->OnGrow.RemoveDynamic(this, &ThisClass::UpdatePlanterBoxInfo);
		}

		if (PlantedCropsGimmick->OnHarvest.IsAlreadyBound(this, &ThisClass::ClearPlanterBoxInfo))
		{
			PlantedCropsGimmick->OnHarvest.RemoveDynamic(this, &ThisClass::ClearPlanterBoxInfo);
		}

		if (PlantedCropsGimmick->OnCropComposted.IsAlreadyBound(this, &ThisClass::ClearPlanterBoxInfo))
		{
			PlantedCropsGimmick->OnCropComposted.RemoveDynamic(this, &ThisClass::ClearPlanterBoxInfo);
		}

		PlantedCropsGimmick = nullptr;
	}

	ClearPlanterBoxInfo();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UPlanterBoxInventoryWidget::UpdatePlanterBoxInfo(const float CurrentGrowthPercent)
{
	if (IsValid(InventoryComponent) && IsValid(PlantedCropsGimmick))
	{
		const FText CropsName = PlantedCropsGimmick->GetGimmickInfoData()->Name;
		CropsNameTextBlock->SetText(CropsName);

		const FString CropsDescription = PlantedCropsGimmick->GetGimmickInfoData()->Description.ToString();

		const FString GrowthPercentTextString = CropsDescription + "\n" + GrowthPercentText.ToString()
			+ FString::Printf(TEXT(" %.1f%%"), CurrentGrowthPercent);

		const FText Text = FText::FromString(GrowthPercentTextString);
		PlanterBoxInfoTextBlock->SetText(Text);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UPlanterBoxInventoryWidget::ClearPlanterBoxInfo()
{
	CropsNameTextBlock->SetText(DefaultNameText);
	PlanterBoxInfoTextBlock->SetText(DefaultInfoText);
}

void UPlanterBoxInventoryWidget::ConnectCropsGimmick()
{
	const AActor* OwnerActor = InventoryComponent->GetOwner();
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(InventoryComponent)) ||
		!CR4S_VALIDATE(LogInventoryUI, IsValid(OwnerActor)))
	{
		return;
	}

	const APlanterBoxGimmick* PlanterBoxGimmick = Cast<APlanterBoxGimmick>(OwnerActor);
	if (IsValid(PlanterBoxGimmick))
	{
		PlantedCropsGimmick = PlanterBoxGimmick->GetPlantedGimmick();
		if (IsValid(PlantedCropsGimmick))
		{
			PlantedCropsGimmick->OnGrow.AddUniqueDynamic(this, &ThisClass::UpdatePlanterBoxInfo);
			PlantedCropsGimmick->OnHarvest.AddUniqueDynamic(this, &ThisClass::ClearPlanterBoxInfo);
			PlantedCropsGimmick->OnCropComposted.AddUniqueDynamic(this, &ThisClass::ClearPlanterBoxInfo);

			UpdatePlanterBoxInfo(PlantedCropsGimmick->GetCurrentGrowthPercent());
		}
		else
		{
			ClearPlanterBoxInfo();
		}
	}
}
