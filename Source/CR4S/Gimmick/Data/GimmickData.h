#pragma once

#include "CoreMinimal.h"
#include "BaseDataInfo.h"

#include "GimmickData.generated.h"

UENUM(BlueprintType)
enum class EGimmickType : uint8
{
	Resources UMETA(DisplayName = "재료"),
	Building UMETA(DisplayName = "건축물"),
	Crops UMETA(DisplayName = "작물")
};

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
struct FBaseGimmickData : public FTableRowBase
{
	GENERATED_BODY()

	FBaseGimmickData()
		: Type(EGimmickType::Resources),
		  GimmickMaxHealth(0.f)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "기본 정보"))
	FBaseDataInfo Info;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "종류"))
	EGimmickType Type;
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
struct FConditionRange
{
	GENERATED_BODY()

	FConditionRange()
		: MinValue(0),
		  MaxValue(0)
	{
	}

	bool IsInRange(const int32 Value) const
	{
		return MinValue <= Value && Value <= MaxValue;
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "최솟값"))
	int32 MinValue;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "최댓값"))
	int32 MaxValue;
};

USTRUCT()
struct FConditionalGrowthBoost
{
	GENERATED_BODY()

	FConditionalGrowthBoost()
		: bIsDay(false),
		  bIsNight(false),
		  AccelerationMultiplier(1.f)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "낮 일때 성장 가속"))
	bool bIsDay;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "밤 일때 성장 가속"))
	bool bIsNight;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 온도 범위"))
	FConditionRange TempRange;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 습도 범위"))
	FConditionRange HumidityRange;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 배율", ClampMin = "1.0"))
	float AccelerationMultiplier;
};

USTRUCT()
struct FGrowthSlowdownData
{
	GENERATED_BODY()

	FGrowthSlowdownData()
		: MinThreshold(0),
		  MaxThreshold(0),
		  SlowdownMultiplier(1.f)
	{
	}

	bool IsInRange(const int32 Value) const
	{
		return MinThreshold <= Value && Value <= MaxThreshold;
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "최소 임계값"))
	int32 MinThreshold;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "최대 임계값"))
	int32 MaxThreshold;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 감속 배율", ClampMin = "0.0", ClampMax = "1.0"))
	float SlowdownMultiplier;
};

USTRUCT(BlueprintType)
struct FCropsGimmickData : public FTableRowBase
{
	GENERATED_BODY()

	FCropsGimmickData()
		: bUseGrowthBoost(false),
		  GrowthRate(1)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "온도로 인한 성장 감속 데이터"))
	FGrowthSlowdownData HeatDamping;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "습도로 인한 성장 감속 데이터"))
	FGrowthSlowdownData HumidityDamping;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 사용 여부"))
	bool bUseGrowthBoost;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 데이터"))
	FConditionalGrowthBoost ConditionalGrowthBoost;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 시간", ToolTip = "단위: 분", ClampMin = "1"))
	int32 GrowthRate;
};
