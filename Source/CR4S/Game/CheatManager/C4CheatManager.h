#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "C4CheatManager.generated.h"

UCLASS()
class CR4S_API UC4CheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	//Cheat Command
	UFUNCTION(exec)
	void AddMinute(int32 Amount);
	UFUNCTION(exec)
	void AddDay(int32 Amount);

	UFUNCTION(exec)
	void SetMonstersHP(const float InHealth);
	
protected:
	//Helper Decelaration
	UPROPERTY()
	TObjectPtr<class UTimeCheatHelper> TimeHelper;

	virtual void InitCheatManager() override;


#pragma region Item And Gimmick

public:
	UFUNCTION(Exec)
	void AddItem(FName RowName, int32 Count) const;

protected:
	UPROPERTY()
	TObjectPtr<class UItemGimmickHelper> ItemGimmickHelper;
	
#pragma endregion
};
