#pragma once

#include "CoreMinimal.h"
#include "CR4S/Gimmick/GimmickObjects/BaseGimmick/BaseGimmick.h"
#include "BaseResourceGimmick.generated.h"

class UDestructibleComponent;

UCLASS()
class CR4S_API ABaseResourceGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override

public:
	ABaseResourceGimmick();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region Components

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UDestructibleComponent* GetDestructibleComponent() const { return DestructibleComponent; }

private:
	UFUNCTION()
	void OnDestroyedGimmick();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UDestructibleComponent> DestructibleComponent;

#pragma endregion
};
