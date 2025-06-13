#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BuildingDurabilityWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class CR4S_API UBuildingDurabilityWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initilaize

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Init")
	void InitWidget(AActor* BuildingActor);
	
#pragma endregion 
	
#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentDurabilityTextBlock;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxDurabilityTextBlock;

#pragma endregion 
};
