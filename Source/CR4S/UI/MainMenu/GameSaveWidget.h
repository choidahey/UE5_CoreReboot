#pragma once

#include "Blueprint/UserWidget.h"
#include "GameSaveWidget.generated.h"

class UButtonWidget;
class UEmptySlotWidget;
class USaveSlotWidget;
class UConfirmWidget;
class USaveGameManager;
class UMainMenuWidget;

UCLASS()
class CR4S_API UGameSaveWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void OpenWindow();

	UPROPERTY()
	TObjectPtr<UMainMenuWidget> MainMenuWidgetRef;

protected:

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
	UFUNCTION()
	void HandleDeleteButtonClicked();
	UFUNCTION()
	void HandleDeleteSlot();
	UFUNCTION()
	void HandleSaveSlotClicked(int32 SlotIndex);

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButtonWidget> BackButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButtonWidget> DeleteButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEmptySlotWidget> SaveSlot0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEmptySlotWidget> SaveSlot1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEmptySlotWidget> SaveSlot2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEmptySlotWidget> SaveSlot3;

#pragma region Widget Class & Widget Instances

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget Class")
	TSubclassOf<UEmptySlotWidget> EmptySlotWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget Class")
	TSubclassOf<USaveSlotWidget> SaveSlotWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget Class")
	TSubclassOf<UConfirmWidget> ConfirmWidgetClass;

	TArray<TObjectPtr<UEmptySlotWidget>> SaveSlotButtons;
	TMap<int32, TObjectPtr<USaveSlotWidget>> SaveSlotsMap;

	TObjectPtr<UConfirmWidget> ConfirmWidget;


#pragma endregion

protected:
	UFUNCTION()
	void RequestEnterGame(int32 SlotIndex);

private:
	
	void CreateEmptySlot(int32 SlotIndex);

	void LogSaveSlotsInfo() const;
	void LoadSaveSlots();
	
	int32 SelectedSlotIndex = -1;

	TObjectPtr<USaveGameManager> SaveGameManager = nullptr;


};
