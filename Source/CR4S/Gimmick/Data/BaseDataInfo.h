#pragma once

#include "CoreMinimal.h"

#include "BaseDataInfo.generated.h"

USTRUCT()
struct FBaseDataInfo
{
	GENERATED_BODY()

	FBaseDataInfo()
	{
	}

	UPROPERTY(EditAnywhere, meta = (DisplayName = "이름"))
	FText Name;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "설명"))
	FText Description;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "아이콘"))
	TObjectPtr<UTexture2D> Icon;
};
