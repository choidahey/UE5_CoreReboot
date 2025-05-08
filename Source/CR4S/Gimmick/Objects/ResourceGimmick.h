#pragma once

#include "CoreMinimal.h"
#include "CR4S/Gimmick/BaseGimmick.h"
#include "ResourceGimmick.generated.h"

class UDestructibleComponent;

UCLASS()
class CR4S_API AResourceGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override

public:
	AResourceGimmick();

#pragma endregion

#pragma region Components

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UDestructibleComponent* GetDestructibleComponent() const { return DestructibleComponent; }

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UDestructibleComponent> DestructibleComponent;

#pragma endregion
};
