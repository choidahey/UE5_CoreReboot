#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ButtonWidget.generated.h"

class UButton;
class UTextBlock;
class USoundBase;

UCLASS()
class CR4S_API UButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnButtonClickedEvent& OnClicked() const { return CR4S_Button->OnClicked; }
	FOnButtonHoverEvent& OnHovered() const { return CR4S_Button->OnHovered; }
	FOnButtonHoverEvent& OnUnhovered() const { return CR4S_Button->OnUnhovered; }

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonHovered();
	//UFUNCTION()
	//void OnButtonUnhovered();
	UFUNCTION()
	void OnButtonClicked();

	virtual void NativePreConstruct() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound|Effect")
	USoundBase* ButtonClickSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound|Effect")
	USoundBase* ButtonHoverSound;

	UPROPERTY(meta = (BindWidget))
	UButton* CR4S_Button;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Button")
	FText ButtonLabel;

private:
	bool bHasHoveredSoundPlayed;

};
