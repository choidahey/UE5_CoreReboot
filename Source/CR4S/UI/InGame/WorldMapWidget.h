#pragma once

#include "Blueprint/UserWidget.h"
#include "WorldMapWidget.generated.h"

class UImage;

UCLASS()
class CR4S_API UWorldMapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateWorldMap();

protected:
	virtual void NativeConstruct() override;


	UFUNCTION(BlueprintCallable, Category = "Map")
	FVector2D WorldToMapPosition(const FVector& WorldLocation) const;

	UPROPERTY(meta = (BindWidget))
	UImage* PlayerIcon;
	UPROPERTY(meta = (BindWidget))
	UImage* MapImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	FVector2D WorldMin = FVector2D(-110000.f, -10000.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	FVector2D WorldMax = FVector2D(150000.f, 120000.f);

private:



	void FindPlayerLocation();



};
