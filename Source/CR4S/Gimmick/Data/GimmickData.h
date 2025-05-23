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

	UPROPERTY(EditAnywhere, meta = (DisplayName = "재료 데이터 행 이름"))
	FName RowName;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "재료 개수", ClampMin = "0"))
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
	UPROPERTY(EditAnywhere, meta = (DisplayName = "기믹 엑터 클래스"))
	TSubclassOf<class ABaseGimmick> GimmickClass;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "재료 목록"))
	TArray<FResourceItemData> ResourceItemDataList;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "기믹 최대 체력", ClampMin = "0.0"))
	float GimmickMaxHealth;
};
