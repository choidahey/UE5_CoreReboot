#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/BaseGimmick/DestructibleGimmick.h"
#include "ResourceGimmick.generated.h"

UCLASS()
class CR4S_API AResourceGimmick : public ADestructibleGimmick
{
	GENERATED_BODY()

#pragma region ADestructibleGimmick Override

public:
	AResourceGimmick();
	virtual void BeginPlay() override;

	virtual void OnGimmickDestroy() override;
#pragma endregion

#pragma region Components

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UGeometryCollectionComponent> GeometryCollectionComponent;

#pragma endregion
};
