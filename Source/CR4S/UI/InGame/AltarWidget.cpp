#include "UI/InGame/AltarWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Gimmick/GimmickObjects/DemoAltar.h"
#include "Game/System/SeasonType.h"

void UAltarWidget::NativeConstruct()
{
	BindWidgets();
}

void UAltarWidget::BindWidgets()
{
	if (BountifulSeasonButton)
	{
		BountifulSeasonButton->OnClicked.AddDynamic(this, &UAltarWidget::OnBountifulSeasonClicked);
	}

	if (FrostSeasonButton)
	{
		FrostSeasonButton->OnClicked.AddDynamic(this, &UAltarWidget::OnFrostSeasonClicked);
	}

	if (RainySeasonButton)
	{
		RainySeasonButton->OnClicked.AddDynamic(this, &UAltarWidget::OnRainySeasonClicked);
	}

	if (DrySeasonButton)
	{
		DrySeasonButton->OnClicked.AddDynamic(this, &UAltarWidget::OnDrylSeasonClicked);
	}

	if (FrostBossButton)
	{
		FrostBossButton->OnClicked.AddDynamic(this, &UAltarWidget::OnFrostBossClicked);
	}
}

void UAltarWidget::OnBountifulSeasonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("BountifulSeason button clicked."));

	if (Altar)
	{
		Altar->RequestChangeWeather(ESeasonType::BountifulSeason);
	}
}

void UAltarWidget::OnFrostSeasonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("FrostSeason button clicked."));

	if (Altar)
	{
		Altar->RequestChangeWeather(ESeasonType::FrostSeason);
	}
}

void UAltarWidget::OnRainySeasonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("RainySeason button clicked."));
	if (Altar)
	{
		Altar->RequestChangeWeather(ESeasonType::RainySeason);
	}
}

void UAltarWidget::OnDrylSeasonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("DrySeason button clicked."));
	if (Altar)
	{
		Altar->RequestChangeWeather(ESeasonType::DrySeason);
	}
}

void UAltarWidget::OnFrostBossClicked()
{
	if (Altar)
	{
		Altar->SummonFrostBoss();
	}
}

void UAltarWidget::ShowAltarUI()
{
	if (ShowUIAnim)
	{
		PlayAnimation(ShowUIAnim);
	}
}

void UAltarWidget::HideAltarUI()
{
	if (ShowUIAnim)
	{
		const float AnimLength = ShowUIAnim->GetEndTime();
		PlayAnimation(ShowUIAnim, AnimLength, 1, EUMGSequencePlayMode::Reverse);
	}
}


