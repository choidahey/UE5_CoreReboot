#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryWidget.h"
#include "RobotInventoryWidget.generated.h"

class UModularRobotStatusComponent;
class AModularRobot;
class UCraftingWidget;
class URecipeSelectWidget;

UENUM(BlueprintType)
enum class ERobotWorkshopMode : uint8
{
	Create UMETA(DisplayName = "생산"),
	Assembly UMETA(DisplayName = "조립")
};

UCLASS()
class CR4S_API URobotInventoryWidget : public UBaseInventoryWidget
{
	GENERATED_BODY()

#pragma region UBaseInventoryWidget Override

public:
	virtual void NativeConstruct() override;

	virtual void InitWidget(ASurvivalHUD* SurvivalHUD, bool bNewCanSort) override;
	virtual void ConnectInventoryComponent(UBaseInventoryComponent* NewInventoryComponent) override;

	virtual void UnBoundWidgetDelegate() override;

	virtual void CloseSlotWidgets() override;
	
#pragma endregion
	
#pragma region RobotWorkshop

public:
	UFUNCTION()
	void OpenRecipeSelectWidget(UTexture2D* Icon, const FText& Name, const FGameplayTag& RecipeTag) const;
	void CloseRecipeSelectWidget() const;

	void CloseCraftingWidget() const;
	
	UFUNCTION()
	void ChangeModeCreate();
	UFUNCTION()
	void ChangeModeAssembly();

	FORCEINLINE AModularRobot* GetModularRobot() const { return ModularRobot; }
	
private:
	void ChangeMode(ERobotWorkshopMode NewRobotWorkshopMode);

	void BindDelegate();

	UFUNCTION()
	void UpdateCurrentWeightText(float NewValue);
	UFUNCTION()
	void UpdateMaxWeightText(float NewValue);
	UFUNCTION()
	void UpdateCurrentArmWeightText(float NewValue);
	UFUNCTION()
	void UpdateMaxArmWeightText(float NewValue);
	
	UPROPERTY(VisibleAnywhere, Category = "RobotWorkshop|Mode")
	ERobotWorkshopMode RobotWorkshopMode;

	UPROPERTY()
	TObjectPtr<AModularRobot> ModularRobot;
	UPROPERTY()
	TObjectPtr<UModularRobotStatusComponent> RobotStatusComponent;
	
#pragma endregion

#pragma region BindWidget

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE UCraftingWidget* GetCraftingWidget() const { return CraftingWidget; }
	
private:
	void UpdateCurrentWeightColor(bool bIsArmWeight) const;
	void UpdateButtonColor() const;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateModeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AssemblyModeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URecipeSelectWidget> RecipeSelectWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingWidget> CraftingWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentWeightText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxWeightText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentArmWeightText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MaxArmWeightText;

	UPROPERTY(EditDefaultsOnly, Category = "RobotInventoryWidget")
	TObjectPtr<UDataTable> ItemRecipeData;

	UPROPERTY(EditDefaultsOnly, Category = "RobotInventoryWidget")
	FText CreateModeText;
	UPROPERTY(EditDefaultsOnly, Category = "RobotInventoryWidget")
	FText AssemblyModeText;

	UPROPERTY(EditDefaultsOnly, Category = "Color")
	FLinearColor CurrentModeButtonColor = FLinearColor::Green;
	
	UPROPERTY(EditDefaultsOnly, Category = "Color")
	FLinearColor DefaultColor = FLinearColor::White;
	UPROPERTY(EditDefaultsOnly, Category = "Color")
	FLinearColor OverColor = FLinearColor::Red;

	int32 CurrentWeight = 0;
	int32 MaxWeight = 0;
	int32 CurrentArmWeight = 0;
	int32 MaxArmWeight = 0;
	
#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRobotWorkshopModeChange, const ERobotWorkshopMode, RobotWorkshopMode);
	UPROPERTY(BlueprintAssignable, Category = "RobotInventoryWidget|Delegate")
	FOnRobotWorkshopModeChange OnRobotWorkshopModeChange;
	
#pragma endregion 
};
