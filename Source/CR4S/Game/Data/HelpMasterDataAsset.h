#pragma once

#include "Engine/DataAsset.h"
#include "HelpMasterDataAsset.generated.h"

UENUM(BlueprintType)
enum class EHelpContentType : uint8
{
	Text,
	Image
};

USTRUCT(BlueprintType)
struct FHelpContentBlock
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EHelpContentType ContentType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ContentType == EHelpContentType::Text", MultiLine = true))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ContentType == EHelpContentType::Image"))
	TSoftObjectPtr<UTexture2D> Image;
};

USTRUCT(BlueprintType)
struct FHelpSubCategoryData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText SubCategoryName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FHelpContentBlock> Contents;
};

USTRUCT(BlueprintType)
struct FHelpMainCategoryData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText MainCategoryName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FHelpSubCategoryData> SubCategories;
};

UCLASS(BlueprintType)
class UHelpMasterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FHelpMainCategoryData> HelpCategories;

	static const FPrimaryAssetType AssetType;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("HelpMasterDataAsset", GetFName());
	}

};
