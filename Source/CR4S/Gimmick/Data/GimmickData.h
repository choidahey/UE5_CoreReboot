#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "GimmickData.generated.h"

USTRUCT(BlueprintType)
struct FResourceItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "재료 데이터 행 이름"))
	FName RowName = NAME_None;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "재료 최소 개수", ClampMin = "0"))
	int32 MinCount = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "재료 최대 개수", ClampMin = "0"))
	int32 MaxCount = 0;
};

USTRUCT(BlueprintType)
struct FGimmickInfoData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "이름"))
	FText Name = FText::FromString("");
	UPROPERTY(EditAnywhere, meta = (DisplayName = "설명"))
	FText Description = FText::FromString("");
	UPROPERTY(EditAnywhere, meta = (DisplayName = "아이콘"))
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "태그"))
	FGameplayTagContainer GimmickTags = FGameplayTagContainer();
	UPROPERTY(EditAnywhere, meta = (DisplayName = "디테일 데이터"))
	FDataTableRowHandle DetailData = FDataTableRowHandle();
	UPROPERTY(EditAnywhere, meta = (DisplayName = "기믹 엑터 클래스"))
	TSubclassOf<class ABaseGimmick> GimmickClass = nullptr;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "기믹 최대 체력", ClampMin = "0.0"))
	int32 GimmickMaxHealth = 0.f;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "재료 목록"))
	TArray<FResourceItemData> Resources;
};

USTRUCT(BlueprintType)
struct FCropsGimmickData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "작물 스태틱 메시 목록"))
	TArray<TObjectPtr<UStaticMesh>> CropsMeshes;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 감속 온도 최소 임계값"))
	int32 HeatMinThreshold = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 감속 온도 최대 임계값"))
	int32 HeatMaxThreshold = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "온도로 인한 성장 감속 배율", ClampMin = "0.0", ClampMax = "1.0"))
	float HeatSlowdownMultiplier = 1.f;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 감속 습도 최소 임계값"))
	int32 HumidityMinThreshold = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 감속 습도 최대 임계값"))
	int32 HumidityMaxThreshold = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "습도로 인한 성장 감속 배율", ClampMin = "0.0", ClampMax = "1.0"))
	float HumiditySlowdownMultiplier = 1.f;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 사용 여부"))
	bool bUseGrowthBoost = false;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "낮 일때 성장 가속", EditCondition = "bUseGrowthBoost"))
	bool bIsDay = false;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "밤 일때 성장 가속", EditCondition = "bUseGrowthBoost"))
	bool bIsNight = false;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "비료 사용시 성장 가속", EditCondition = "bUseGrowthBoost"))
	bool bUseFertilizer = false;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 온도 최솟값", EditCondition = "bUseGrowthBoost"))
	int32 HeatMinValue = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 온도 최댓값", EditCondition = "bUseGrowthBoost"))
	int32 HeatMaxValue = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 습도 최솟값", EditCondition = "bUseGrowthBoost"))
	int32 HumidityMinValue = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 습도 최댓값", EditCondition = "bUseGrowthBoost"))
	int32 HumidityMaxValue = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 가속 배율", ClampMin = "1.0", EditCondition = "bUseGrowthBoost"))
	float AccelerationMultiplier = 1.f;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "성장 시간", ToolTip = "단위: 분", ClampMin = "1"))
	int32 GrowthRate = 1;
};
