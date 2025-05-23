#pragma once

#include "CoreMinimal.h"
#include "BaseDataInfo.h"

#include "GimmickData.generated.h"

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
struct FBaseGimmickData : public FTableRowBase
{
	GENERATED_BODY()

	FBaseGimmickData()
		: GimmickMaxHealth(0.f)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "기본 정보"))
	FBaseDataInfo Info;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABaseGimmick> GimmickClass;
	UPROPERTY(EditAnywhere)
	TArray<FResourceItemData> ResourceItemDataList;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
	float GimmickMaxHealth;
};
