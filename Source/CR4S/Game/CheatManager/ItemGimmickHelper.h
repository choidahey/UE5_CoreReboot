#pragma once

#include "CoreMinimal.h"

#include "ItemGimmickHelper.generated.h"

UCLASS()
class CR4S_API UItemGimmickHelper : public UObject
{
	GENERATED_BODY()
	
#pragma region AddItem

public:
	UFUNCTION(Exec)
	void AddItem(FName RowName, int32 Count) const;
	
#pragma endregion 
};
