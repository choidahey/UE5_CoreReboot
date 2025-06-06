#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "ItemCheatManager.generated.h"

UCLASS()
class CR4S_API UItemCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
#pragma region AddItem

public:
	UFUNCTION(Exec)
	void AddItem(FName RowName, int32 Count) const;
	
#pragma endregion 
};
