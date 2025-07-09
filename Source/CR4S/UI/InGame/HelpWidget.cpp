#include "UI/InGame/HelpWidget.h"
#include "UI/Common/SubCategoryButtonWidget.h"
#include "UI/Common/CategoryButtonWidget.h"
#include "UI/Common/BaseWindowWidget.h"
#include "Components/ScrollBox.h"
#include "Components/HorizontalBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Spacer.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/Font.h"
#include "CR4S.h"

void UHelpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!WindowWidget->OnBackClicked.IsAlreadyBound(this, &UHelpWidget::HandleCloseWindow))
	{
		WindowWidget->OnBackClicked.AddDynamic(this, &UHelpWidget::HandleCloseWindow);
	}

	InitHelpWidget();
}

//void UHelpWidget::LoadHelpAsset(TFunction<void(UHelpMasterDataAsset*)> Callback)
//{
//	const FPrimaryAssetId AssetId = FPrimaryAssetId(UHelpMasterDataAsset::AssetType, TEXT("DA_Help"));
//
//	UAssetManager::Get().LoadPrimaryAsset(
//		AssetId,
//		TArray<FName>(),
//		FStreamableDelegate::CreateWeakLambda(this, [this, AssetId, Callback]()
//			{
//				UHelpMasterDataAsset* LoadedAsset =
//					Cast<UHelpMasterDataAsset>(UAssetManager::Get().GetPrimaryAssetObject(AssetId));
//
//				Callback(LoadedAsset);
//			})
//	);
//}

void UHelpWidget::InitHelpWidget()
{
	LoadCategory();

	UHelpMasterDataAsset* LoadedAsset = LoadHelpAsset();
	if (!LoadedAsset || LoadedAsset->HelpCategories.Num() == 0) return;

	const FHelpMainCategoryData& FirstMainCategory = LoadedAsset->HelpCategories[0];

	LoadSubCategory(FirstMainCategory.MainCategoryName);

	if (FirstMainCategory.SubCategories.Num() == 0) return;

	const FHelpSubCategoryData& FirstSubCategory = FirstMainCategory.SubCategories[0];

	LoadHelpContent(FirstSubCategory.SubCategoryName);

	CurrentMainCategory = FirstMainCategory.MainCategoryName;
	CurrentSubCategory = FirstSubCategory.SubCategoryName;
}

UHelpMasterDataAsset* UHelpWidget::LoadHelpAsset()
{
	const FPrimaryAssetId AssetId = FPrimaryAssetId(UHelpMasterDataAsset::AssetType, TEXT("DA_Help"));

	UObject* LoadedObject = UAssetManager::Get().GetPrimaryAssetObject(AssetId);

	if (!LoadedObject)
	{
		return Cast<UHelpMasterDataAsset>(StaticLoadObject(UHelpMasterDataAsset::StaticClass(), nullptr, TEXT("/Game/CR4S/_Data/Game/DA_Help.DA_Help")));
	}

	return Cast<UHelpMasterDataAsset>(LoadedObject);
}


void UHelpWidget::LoadCategory()
{
	UHelpMasterDataAsset* LoadedAsset = LoadHelpAsset();
	if (!LoadedAsset) return;

	for (const FHelpMainCategoryData& MainCategory : LoadedAsset->HelpCategories)
	{
		AddCategory(MainCategory.MainCategoryName);
	}
}

void UHelpWidget::LoadSubCategory(const FText& InMainCategoryName)
{
	CachedSubCategoryContents.Empty();

	UHelpMasterDataAsset* LoadedAsset = LoadHelpAsset();
	if (!LoadedAsset) return;

	for (const FHelpMainCategoryData& MainCategory : LoadedAsset->HelpCategories)
	{
		if (MainCategory.MainCategoryName.EqualTo(InMainCategoryName))
		{
			for (const FHelpSubCategoryData& SubCategory : MainCategory.SubCategories)
			{
				AddSubCategory(SubCategory.SubCategoryName);
				CachedSubCategoryContents.Add(FName(*SubCategory.SubCategoryName.ToString()), SubCategory.Contents);
			}
			break;
		}
	}
}

//void UHelpWidget::LoadSubCategory(const FText& InMainCategoryName)
//{
//	CachedSubCategoryContents.Empty();
//
//	LoadHelpAsset([this, InMainCategoryName](UHelpMasterDataAsset* LoadedAsset)
//		{
//			if (!LoadedAsset) return;
//
//			for (const FHelpMainCategoryData& MainCategory : LoadedAsset->HelpCategories)
//			{
//				if (MainCategory.MainCategoryName.EqualTo(InMainCategoryName))
//				{
//					for (const FHelpSubCategoryData& SubCategory : MainCategory.SubCategories)
//					{
//						AddSubCategory(SubCategory.SubCategoryName);
//
//						CachedSubCategoryContents.Add(FName(*SubCategory.SubCategoryName.ToString()), SubCategory.Contents);
//					}
//					break;
//				}
//			}
//		});
//}

void UHelpWidget::LoadHelpContent(const FText& InSubCategoryName)
{
	const TArray<FHelpContentBlock>* Contents = CachedSubCategoryContents.Find(FName(*InSubCategoryName.ToString()));
	if (!Contents) return;

	for (const FHelpContentBlock& ContentBlock : *Contents)
	{
		if (ContentBlock.ContentType == EHelpContentType::Text)
		{
			AddTextContent(ContentBlock.Text);
		}
		else if (ContentBlock.ContentType == EHelpContentType::Image)
		{
			AddImageContent(ContentBlock.Image);
		}
	}
}


void UHelpWidget::AddCategory(const FText& InCategoryName)
{
	if (!CategoryButtonWidgetClass || !MainCategoryList) return;

	UCategoryButtonWidget* NewCategoryButton = CreateWidget<UCategoryButtonWidget>(this, CategoryButtonWidgetClass);
	if (!NewCategoryButton) return;

	NewCategoryButton->SetCategoryLabel(InCategoryName);
	MainCategoryList->AddChild(NewCategoryButton);
	MainCategoryList->AddChild(CreateSpacer(0.f, MainCategorySpacing));

	NewCategoryButton->OnCategoryClicked.AddDynamic(this, &UHelpWidget::OnMainCategoryClicked_Internal);
}

void UHelpWidget::AddSubCategory(const FText& InSubCategoryName)
{
	if (!SubCategoryButtonWidgetClass || !SubCategoryList) return;

	USubCategoryButtonWidget* NewSubCategoryButton = CreateWidget<USubCategoryButtonWidget>(this, SubCategoryButtonWidgetClass);
	if (!NewSubCategoryButton) return;

	NewSubCategoryButton->SetSubCategoryLabel(InSubCategoryName);
	SubCategoryList->AddChild(NewSubCategoryButton);
	SubCategoryList->AddChild(CreateSpacer(SubCategorySpacing, 0.f));

	NewSubCategoryButton->OnSubCategoryClicked.AddDynamic(this, &UHelpWidget::OnSubCategoryClicked_Internal);
}

void UHelpWidget::OnMainCategoryClicked_Internal(const FText& ClickedCategoryName)
{
	if (!ClickedCategoryName.EqualTo(CurrentMainCategory))
	{
		ClearSubCategoryList();
		ClearHelpContent();

		LoadSubCategory(ClickedCategoryName);

		CurrentMainCategory = ClickedCategoryName;

		const TArray<FHelpContentBlock>* FirstSubCategoryContents = nullptr;
		for (const auto& Pair : CachedSubCategoryContents)
		{
			CurrentSubCategory = FText::FromName(Pair.Key);
			LoadHelpContent(CurrentSubCategory);
			break;
		}
	}
}

void UHelpWidget::OnSubCategoryClicked_Internal(const FText& ClickedCategoryName)
{
	if (!ClickedCategoryName.EqualTo(CurrentSubCategory))
	{
		ClearHelpContent();

		LoadHelpContent(ClickedCategoryName);

		CurrentSubCategory = ClickedCategoryName;
	}

}



void UHelpWidget::AddImageContent(TSoftObjectPtr<UTexture2D> ImageContent)
{
	//if (!HelpContent) return;

	//FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	//Streamable.RequestAsyncLoad(ImageContent.ToSoftObjectPath(), [=]()
	//	{
	//		UTexture2D* LoadedTexture = ImageContent.Get();
	//		if (LoadedTexture)
	//		{
	//			UImage* ImageWidget = NewObject<UImage>(this);
	//			ImageWidget->SetBrushFromTexture(LoadedTexture);
	//			ImageWidget->SetBrushSize(FVector2D(512, 512)); // AdjustSize
	//			HelpContent->AddChild(ImageWidget);
	//		}
	//	});
}

void UHelpWidget::AddTextContent(const FText& TextContent)
{
	if (!HelpContent) return;

	UTextBlock* TextWidget = NewObject<UTextBlock>(this);
	TextWidget->SetText(TextContent);

	if (HelpFont)
	{
		FSlateFontInfo FontInfo;
		FontInfo.Size = 18;
		FontInfo.FontObject = HelpFont;

		TextWidget->SetFont(FontInfo);
	}

	FSlateColor FontColor(FLinearColor(0.8f, 0.666667f, 0.47451f, 1.f));
	TextWidget->SetColorAndOpacity(FontColor);

	TextWidget->SetAutoWrapText(true);


	UWidget* TopSpacer = CreateSpacer(0.f, 0.f);
	UVerticalBoxSlot* TopSlot = HelpContent->AddChildToVerticalBox(TopSpacer);
	if (TopSlot)
	{
		TopSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	HelpContent->AddChild(TextWidget);

	UWidget* BottomSpacer = CreateSpacer(0.f, 0.f);
	UVerticalBoxSlot* BottomSlot = HelpContent->AddChildToVerticalBox(BottomSpacer);
	if (BottomSlot)
	{
		BottomSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

}

void UHelpWidget::ClearMainCategoryList()
{
	if (MainCategoryList) MainCategoryList->ClearChildren();
}

void UHelpWidget::ClearSubCategoryList()
{
	if (SubCategoryList) SubCategoryList->ClearChildren();
}

void UHelpWidget::ClearHelpContent()
{
	if (HelpContent) HelpContent->ClearChildren();
}

UWidget* UHelpWidget::CreateSpacer(float Width, float Height)
{
	USpacer* Spacer = NewObject<USpacer>(this);
	if (Spacer)
	{
		Spacer->SetSize(FVector2D(Width, Height));
	}
	return Spacer;
}

void UHelpWidget::HandleCloseWindow()
{
	SetVisibility(ESlateVisibility::Hidden);

	OnHelpClosed.Broadcast();

	RemoveFromParent();
}

void UHelpWidget::NativeDestruct()
{
	if (WindowWidget)
	{
		WindowWidget->OnBackClicked.RemoveDynamic(this, &UHelpWidget::HandleCloseWindow);
	}

	Super::NativeDestruct();
}
