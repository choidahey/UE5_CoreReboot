#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryWidget.h"
#include "RobotInventoryWidget.generated.h"

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
	
private:
	void ChangeMode(ERobotWorkshopMode NewRobotWorkshopMode);
	
	UPROPERTY(VisibleAnywhere, Category = "RobotWorkshop|Mode")
	ERobotWorkshopMode RobotWorkshopMode;
	
#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CreateModeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AssemblyModeButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URecipeSelectWidget> RecipeSelectWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCraftingWidget> CraftingWidget;

	UPROPERTY(EditDefaultsOnly, Category = "RobotInventoryWidget")
	TObjectPtr<UDataTable> ItemRecipeData;

	UPROPERTY(EditDefaultsOnly, Category = "RobotInventoryWidget")
	FText CreateModeText;
	UPROPERTY(EditDefaultsOnly, Category = "RobotInventoryWidget")
	FText AssemblyModeText;
	
#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRobotWorkshopModeChange, const ERobotWorkshopMode, RobotWorkshopMode);
	UPROPERTY(BlueprintAssignable, Category = "RobotInventoryWidget|Delegate")
	FOnRobotWorkshopModeChange OnRobotWorkshopModeChange;
	
#pragma endregion 
};
