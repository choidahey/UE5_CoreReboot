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

	UPROPERTY(EditAnywhere, meta = (DisplayName = "스태틱 메시"))
	TObjectPtr<UStaticMesh> StaticMesh = nullptr;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "애니메이션 몽타주"))
	TObjectPtr<UAnimMontage> AnimMontage = nullptr;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "공격력", ClampMin = "0"))
	int32 Damage = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "기절 수치", ClampMin = "0"))
	int32 StunValue = 0;
};

USTRUCT(BlueprintType)
struct FBossMonsterSpawnItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "보스 몬스터 데이터"))
	FDataTableRowHandle BossMonsterData = FDataTableRowHandle();
};

USTRUCT(BlueprintType)
struct FHarvestedMaterial
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (DisplayName = "수확 재료 데이터"))
	FDataTableRowHandle MaterialData = FDataTableRowHandle();
	UPROPERTY(EditAnywhere, meta = (DisplayName = "수확 재료 개수", ClampMin = "0"))
	int32 Count = 0;
};

USTRUCT(BlueprintType)
struct FAnimalItemData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (DisplayName = "수확 재료 목록"))
	TArray<FHarvestedMaterial> HarvestedMaterials;
};

USTRUCT(BlueprintType)
struct FConsumableItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "심기 가능 여부"))
	bool bCanPlant = false;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "작물 기믹 데이터"))
	FDataTableRowHandle CropsGimmickData = FDataTableRowHandle();
	UPROPERTY(EditAnywhere, meta = (DisplayName = "애니메이션 몽타주"))
	TObjectPtr<UAnimMontage> AnimMontage = nullptr;
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
