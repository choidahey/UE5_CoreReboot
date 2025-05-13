#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseGimmick.generated.h"

struct FResourceItemData;

UCLASS()
class CR4S_API ABaseGimmick : public AActor
{
	GENERATED_BODY()

#pragma region AActor Override

public:
	ABaseGimmick();

#pragma endregion

#pragma region Components

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

#pragma endregion

#pragma region GimmickData

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Data")
	FORCEINLINE FName GetGimmickDataRowName() const { return GimmickDataRowName; }
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "GimmickData")
	FName GimmickDataRowName;

#pragma endregion
};
