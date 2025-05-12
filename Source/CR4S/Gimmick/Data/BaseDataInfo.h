#pragma once

#include "CoreMinimal.h"

#include "BaseDataInfo.generated.h"

USTRUCT(BlueprintType)
struct FBaseDataInfo : public FTableRowBase
{
	GENERATED_BODY()

	FBaseDataInfo()
	{
	}

	UPROPERTY(EditAnywhere)
	FText Name;
	UPROPERTY(EditAnywhere)
	FText Description;
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UTexture2D> Icon;
};

USTRUCT(BlueprintType)
struct FBaseItemData : public FBaseDataInfo
{
	GENERATED_BODY()

	FBaseItemData()
		: MaxStack(0)
	{
	}

	UPROPERTY(EditAnywhere)
	int32 MaxStack;
};

USTRUCT(BlueprintType)
struct FResourceItemData
{
	GENERATED_BODY()

	FResourceItemData()
		: Count(0)
	{
	}

	UPROPERTY(EditAnywhere)
	FName RowName;
	UPROPERTY(EditAnywhere)
	int32 Count;
};

USTRUCT(BlueprintType)
struct FBaseGimmickData : public FBaseDataInfo
{
	GENERATED_BODY()

	FBaseGimmickData()
	{
	}

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<class ABaseGimmick> GimmickClass;
	UPROPERTY(EditAnywhere)
	TArray<FResourceItemData> ResourceItemDataList;
};
