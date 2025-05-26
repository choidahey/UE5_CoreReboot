#pragma once

#include "CoreMinimal.h"
#include "BaseDataInfo.h"

#include "ItemData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Resources UMETA(DisplayName = "재료"),
	Building UMETA(DisplayName = "건축물"),
	Tool UMETA(DisplayName = "도구"),
	Spawn UMETA(DisplayName = "소환"),
	Animal UMETA(DisplayName = "동물"),
	Consumable UMETA(DisplayName = "소모품")
};

USTRUCT(BlueprintType)
struct FItemInfoData : public FTableRowBase
{
	GENERATED_BODY()

	FItemInfoData()
		: Type(EItemType::Resources),
		  MaxStackCount(64)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "기본 정보"))
	FBaseDataInfo Info;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "종류"))
	EItemType Type;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "최대 중첩 개수", ClampMin = "1", ClampMax = "9999"))
	int32 MaxStackCount;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "디테일 데이터"))
	FDataTableRowHandle DetailData;
};

USTRUCT(BlueprintType)
struct FToolItemData : public FTableRowBase
{
	GENERATED_BODY()

	FToolItemData()
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "스태틱 메시"))
	TObjectPtr<UStaticMesh> StaticMesh;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "애니메이션 몽타주"))
	TObjectPtr<UAnimMontage> AnimMontage;
};

USTRUCT(BlueprintType)
struct FBossMonsterSpawnItemData : public FTableRowBase
{
	GENERATED_BODY()

	FBossMonsterSpawnItemData()
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "보스 몬스터 데이터"))
	FDataTableRowHandle BossMonsterData;
};

USTRUCT(BlueprintType)
struct FHarvestedMaterial
{
	GENERATED_BODY()

	FHarvestedMaterial()
		: Count(0)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "수확 재료 데이터"))
	FDataTableRowHandle MaterialData;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "수확 재료 개수", ClampMin = "0"))
	int32 Count;
};

USTRUCT(BlueprintType)
struct FAnimalItemData : public FTableRowBase
{
	GENERATED_BODY()

	FAnimalItemData()
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "수확 재료 목록"))
	TArray<FHarvestedMaterial> HarvestedMaterials;
};

USTRUCT(BlueprintType)
struct FResistanceEffect
{
	GENERATED_BODY()

	FResistanceEffect()
		: Value(0),
		  Duration(1)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "저항 증감 수치"))
	int32 Value;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "저항 증감 지속 시간", ToolTip = "단위: 분", ClampMin = "1"))
	int32 Duration;
};

USTRUCT(BlueprintType)
struct FConsumableItemEffect
{
	GENERATED_BODY()

	FConsumableItemEffect()
		: HungerRestore(0),
		  HealthRestore(0)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "허기 증감 수치"))
	int32 HungerRestore;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "체력 증감 수치"))
	int32 HealthRestore;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "더위 저항 데이터"))
	FResistanceEffect HeatResistance;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "습도 저항 데이터"))
	FResistanceEffect HumidityResistance;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "추위 저항 데이터"))
	FResistanceEffect ColdResistance;
};

USTRUCT(BlueprintType)
struct FConsumableItemData : public FTableRowBase
{
	GENERATED_BODY()

	FConsumableItemData()
		: bCanPlant(false),
		  ShelfLife(0)
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "심기 가능 여부"))
	bool bCanPlant;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "작물 기믹 데이터"))
	FDataTableRowHandle CropsGimmickData;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "애니메이션 몽타주"))
	TObjectPtr<UAnimMontage> AnimMontage;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "효과 목록"))
	FConsumableItemEffect Effect;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "보관 시간", ToolTip = "단위: 분", ClampMin = "0"))
	int32 ShelfLife;
};
