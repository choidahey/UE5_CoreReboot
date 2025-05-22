#pragma once

#include "CoreMinimal.h"

#include "BaseDataInfo.generated.h"

USTRUCT()
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
	TObjectPtr<UTexture2D> Icon;
};

USTRUCT(BlueprintType)
struct FBaseItemData : public FBaseDataInfo
{
	GENERATED_BODY()

	FBaseItemData()
		: MaxStack(1)
	{
	}

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
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
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	int32 Count;
};

USTRUCT(BlueprintType)
struct FBaseGimmickData : public FBaseDataInfo
{
	GENERATED_BODY()

	FBaseGimmickData()
		: GimmickMaxHealth(0.f)
	{
	}

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABaseGimmick> GimmickClass;
	UPROPERTY(EditAnywhere)
	TArray<FResourceItemData> ResourceItemDataList;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float GimmickMaxHealth;
};
