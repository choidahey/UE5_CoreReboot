#pragma once

#include "Blueprint/UserWidget.h"
#include "GameIntroWidget.generated.h"

class USizeBox;
class UImage;

DECLARE_DELEGATE(FOnIntroFinished);

UCLASS()
class CR4S_API UGameIntroWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intro")
	TArray<UTexture2D*> IntroTextures;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> LogoImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> LogoSizeBox;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	int32 CurrentIndex = 0;
	float DisplayTimer = 0.f;
	float FadeAlpha = 0.f;
	bool bFadingIn = true;
	bool bFinished = false;

	UPROPERTY(EditAnywhere, Category = "Intro")
	float FadeDuration = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Intro")
	float HoldDuration = 1.5f;

	void AdvanceToNextTexture();

public:
	FOnIntroFinished OnIntroFinished;
};
