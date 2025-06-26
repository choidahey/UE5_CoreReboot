#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct FItemInfoData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "이름"))
	FText Name = FText::FromString("");
	UPROPERTY(EditAnywhere, meta = (DisplayName = "설명"))
	FText Description = FText::FromString("");
	UPROPERTY(EditAnywhere, meta = (DisplayName = "아이콘"))
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "사용 효과음"))
	TObjectPtr<USoundBase> UseSound = nullptr;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "태그"))
	FGameplayTagContainer ItemTags = FGameplayTagContainer();
	UPROPERTY(EditAnywhere, meta = (DisplayName = "최대 중첩 개수", ClampMin = "1", ClampMax = "9999"))
	int32 MaxStackCount = 64;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "디테일 데이터"))
	FDataTableRowHandle DetailData = FDataTableRowHandle();
};

USTRUCT(BlueprintType)
struct FToolItemData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (DisplayName = "도구 태그"))
	FGameplayTag ToolTag = FGameplayTag();
	UPROPERTY(EditAnywhere, meta = (DisplayName = "도구 공격력"))
	int32 Damage = 0;
};

USTRUCT(BlueprintType)
struct FConsumableItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "심기 가능 여부"))
	bool bCanPlant = false;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "작물 기믹 데이터"))
	FDataTableRowHandle CropsGimmickData = FDataTableRowHandle();
	UPROPERTY(EditAnywhere, meta = (DisplayName = "허기 증감 수치"))
	int32 HungerRestore = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "체력 증감 수치"))
	int32 HealthRestore = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "더위 저항 증감 수치"))
	int32 HeatResistanceValue = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "더위 저항 증감 지속 시간", ToolTip = "단위: 분", ClampMin = "1"))
	int32 HeatResistanceDuration = 1;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "습도 증감 수치"))
	int32 HumidityResistanceValue = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "습도 저항 증감 지속 시간", ToolTip = "단위: 분", ClampMin = "1"))
	int32 HumidityResistanceDuration = 1;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "추위 저항 증감 수치"))
	int32 ColdResistanceValue = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "추위 저항 증감 지속 시간", ToolTip = "단위: 분", ClampMin = "1"))
	int32 ColdResistanceDuration = 1;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "보관 시간", ToolTip = "단위: 분", ClampMin = "0"))
	int32 ShelfLife = 0;
};

USTRUCT(BlueprintType)
struct FHelperBotItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "자동화 로봇 클래스"))
	TSubclassOf<class ABaseHelperBot> HelperBotClass = nullptr;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "스폰 오프셋", ToolTip = "X: OffsetFoward, Z: OffsetUp"))
	FVector SpawnOffset = FVector(0.f, 0.f, 0.f);
};

USTRUCT(BlueprintType)
struct FRobotPartsInfoFormatData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "출력 서식", MultiLine = "true"))
	FText FormatPattern;
};
