#include "UI/MainMenu/GameIntroWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void UGameIntroWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentIndex = 0;
	DisplayTimer = 0.f;
	FadeAlpha = 0.f;
	bFadingIn = true;
	bFinished = false;

	if (IntroTextures.IsValidIndex(CurrentIndex) && LogoImage)
	{
		UTexture2D* Tex = IntroTextures[CurrentIndex];
		LogoImage->SetBrushFromTexture(Tex);
		LogoImage->SetOpacity(0.f);

		if (Tex && LogoSizeBox)
		{
			float TexWidth = (float)Tex->GetSizeX();
			float TexHeight = (float)Tex->GetSizeY();

			LogoSizeBox->SetWidthOverride(TexWidth);
			LogoSizeBox->SetHeightOverride(TexHeight);
		}
	}
}

void UGameIntroWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bFinished || !IntroTextures.IsValidIndex(CurrentIndex) || !LogoImage)
		return;

	DisplayTimer += InDeltaTime;

	if (bFadingIn)
	{
		FadeAlpha = FMath::Clamp(DisplayTimer / FadeDuration, 0.f, 1.f);
		LogoImage->SetOpacity(FadeAlpha);

		if (FadeAlpha >= 1.f)
		{
			bFadingIn = false;
			DisplayTimer = 0.f;
		}
	}
	else
	{
		if (DisplayTimer < HoldDuration)
		{
			LogoImage->SetOpacity(1.f);
		}
		else
		{
			float FadeOutTime = DisplayTimer - HoldDuration;
			FadeAlpha = FMath::Clamp(1.f - (FadeOutTime / FadeDuration), 0.f, 1.f);
			LogoImage->SetOpacity(FadeAlpha);

			if (FadeAlpha <= 0.f)
			{
				AdvanceToNextTexture();
			}
		}
	}
}

void UGameIntroWidget::AdvanceToNextTexture()
{
	CurrentIndex++;
	DisplayTimer = 0.f;
	bFadingIn = true;

	if (IntroTextures.IsValidIndex(CurrentIndex))
	{
		UTexture2D* Tex = IntroTextures[CurrentIndex];
		LogoImage->SetBrushFromTexture(Tex);
		LogoImage->SetOpacity(0.f);

		if (Tex && LogoSizeBox)
		{
			float TexWidth = (float)Tex->GetSizeX();
			float TexHeight = (float)Tex->GetSizeY();

			LogoSizeBox->SetWidthOverride(TexWidth);
			LogoSizeBox->SetHeightOverride(TexHeight);
		}
	}
	else
	{
		bFinished = true;
		OnIntroFinished.ExecuteIfBound();
		RemoveFromParent();
	}
}
