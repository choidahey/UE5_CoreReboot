#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryComponent.h"
#include "RobotInventoryComponent.generated.h"

class AModularRobot;
class APlayerCharacter;
class UPlayerInventoryComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API URobotInventoryComponent : public UBaseInventoryComponent
{
	GENERATED_BODY()

#pragma region UBaseInventoryComponent Override
	
public:
	URobotInventoryComponent();
	
	virtual FAddItemResult AddItem(FName RowName, int32 Count, UBaseInventoryItem* OriginItem = nullptr) override;
	
#pragma endregion
	
#pragma region PlayerInventoryComponent

public:
	void UpdatePlayerInventoryComponent(const APlayerCharacter* MountedCharacter);
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;
	
#pragma endregion

#pragma region Check Parts

public:
	UFUNCTION(BlueprintPure, Category = "RobotInventoryComponent")
	bool CanAssembleRobot() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Filter")
	FGameplayTagQuery RequirementTagQuery;
	
#pragma endregion 
};
