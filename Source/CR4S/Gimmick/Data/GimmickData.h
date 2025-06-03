#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GimmickData.generated.h"

USTRUCT(BlueprintType)
struct FResourceItemData
{
	GENERATED_BODY()

	FResourceItemData()
		: MinCount(0),
		  MaxCount(0)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "재료 데이터 행 이름"))
	FName RowName;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "재료 최소 개수", ClampMin = "0"))
	int32 MinCount;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "재료 최대 개수", ClampMin = "0"))
	int32 MaxCount;
};

USTRUCT(BlueprintType)
struct FGimmickInfoData : public FTableRowBase
{
	GENERATED_BODY()

	FGimmickInfoData()
		: GimmickMaxHealth(0.f)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "이름"))
	FText Name;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "설명"))
	FText Description;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "아이콘"))
	TObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "태그"))
	FGameplayTagContainer GimmickTags;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "디테일 데이터"))
	FDataTableRowHandle DetailData;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "기믹 엑터 클래스"))
	TSubclassOf<class ABaseGimmick> GimmickClass;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "기믹 최대 체력", ClampMin = "0.0"))
	int32 GimmickMaxHealth;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "재료 목록"))
	TArray<FResourceItemData> Resources;
};

USTRUCT(BlueprintType)
struct FCropsGimmickData : public FTableRowBase
{
	GENERATED_BODY()

	FCropsGimmickData()
		: HeatMinThreshold(0),
		  HeatMaxThreshold(0),
		  HeatSlowdownMultiplier(1.f),
		  HumidityMinThreshold(0),
		  HumidityMaxThreshold(0),
		  HumiditySlowdownMultiplier(1.f),
		  bUseGrowthBoost(false),
		  bIsDay(false),
		  bIsNight(false),
		  HeatMinValue(0),
		  HeatMaxValue(0),
		  HumidityMinValue(0),
		  HumidityMaxValue(0),
		  AccelerationMultiplier(1.f),
		  GrowthRate(1)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 감속 온도 최소 임계값"))
	int32 HeatMinThreshold;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 감속 온도 최대 임계값"))
	int32 HeatMaxThreshold;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "온도로 인한 성장 감속 배율", ClampMin = "0.0", ClampMax = "1.0"))
	float HeatSlowdownMultiplier;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 감속 습도 최소 임계값"))
	int32 HumidityMinThreshold;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 감속 습도 최대 임계값"))
	int32 HumidityMaxThreshold;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "습도로 인한 성장 감속 배율", ClampMin = "0.0", ClampMax = "1.0"))
	float HumiditySlowdownMultiplier;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 사용 여부"))
	bool bUseGrowthBoost;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "낮 일때 성장 가속"))
	bool bIsDay;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "밤 일때 성장 가속"))
	bool bIsNight;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 온도 최솟값"))
	int32 HeatMinValue;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 온도 최댓값"))
	int32 HeatMaxValue;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 습도 최솟값"))
	int32 HumidityMinValue;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 습도 최댓값"))
	int32 HumidityMaxValue;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 배율", ClampMin = "1.0"))
	float AccelerationMultiplier;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 시간", ToolTip = "단위: 분", ClampMin = "1"))
	int32 GrowthRate;
};
