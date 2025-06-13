#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BaseBuildingGimmick.generated.h"

class UItemGimmickSubsystem;
class UWidgetComponent;
class UBuildingDurabilityWidget;

UCLASS(BlueprintType, Blueprintable)
class CR4S_API ABaseBuildingGimmick : public AActor
{
	GENERATED_BODY()

#pragma region AActor Override

public:
	ABaseBuildingGimmick();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region Components

private:
	UPROPERTY(VisibleAnywhere, Category = "BuildingGimmick|Components")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "BuildingGimmick|Components")
	TObjectPtr<UWidgetComponent> BuildingDurabilityWidgetComponent;
	
#pragma endregion 

#pragma region BuildingGimmick

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "BuildingGimmick|Durability")
	void OnGetResources(AActor* InventoryOwnerActor);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "BuildingGimmick|Durability")
	void OnRepairBuilding(const float AddDurability);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "BuildingGimmick|Durability")
	float GetDurabilityRatio() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuildingGimmick")
	TObjectPtr<UItemGimmickSubsystem> ItemGimmickSubsystem;

#pragma endregion

#pragma region DurabilityWidget

protected:
	UFUNCTION(BlueprintCallable, Category = "BuildingGimmick|DurabilityWidget")
	void ToggleDurabilityWidget(bool bIsOpen);

private:

	UPROPERTY(BlueprintReadOnly, Category = "BuildingGimmick|DurabilityWidget", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBuildingDurabilityWidget> BuildingDurabilityWidgetInstance;

#pragma endregion
};
