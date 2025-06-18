#pragma once

#include "Blueprint/UserWidget.h"
#include "GameSaveWidget.generated.h"

class UButton;
class UButtonWidget;
class USaveSlotWidget;
class UConfirmWidget;
class USaveGameManager;

UCLASS()
class CR4S_API UGameSaveWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	TObjectPtr<UUserWidget> ParentWidgetRef;

	UFUNCTION()
	void HandleOpenWindow();
	UFUNCTION()
	void HandleCloseWindow();

	UFUNCTION()
	void HandleStartNewGame();
	UFUNCTION()
	void HandleCloseConfirmWidget();

	UFUNCTION()
	void HandleEmptySlotSelected(int32 SlotIndex);
	UFUNCTION()
	void HandleEmptySlot0() { HandleEmptySlotSelected(0); }
	UFUNCTION()
	void HandleEmptySlot1() { HandleEmptySlotSelected(1); }
	UFUNCTION()
	void HandleEmptySlot2() { HandleEmptySlotSelected(2); }
	UFUNCTION()
	void HandleEmptySlot3() { HandleEmptySlotSelected(3); }


protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButtonWidget> BackButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SaveSlot1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SaveSlot2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SaveSlot3;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SaveSlot4;

#pragma region Widget Class & Widget Instances

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget Class")
	TSubclassOf<USaveSlotWidget> SaveSlotWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget Class")
	TSubclassOf<UConfirmWidget> ConfirmWidgetClass;

	TArray<TObjectPtr<UButton>> SaveSlotButtons;
	TArray<TObjectPtr<USaveSlotWidget>> SaveSlots;

	TObjectPtr<UConfirmWidget> ConfirmWidget;


#pragma endregion

private:
	int32 SelectedSlotIndex = -1;

	TObjectPtr<USaveGameManager> SaveGameManager = nullptr;
};
