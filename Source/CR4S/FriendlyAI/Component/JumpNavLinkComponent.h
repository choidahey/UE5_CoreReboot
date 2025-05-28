#pragma once

#include "CoreMinimal.h"
#include "NavLinkCustomComponent.h"
#include "Navigation/PathFollowingComponent.h" 
#include "JumpNavLinkComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UJumpNavLinkComponent : public UNavLinkCustomComponent
{
	GENERATED_BODY()

public:
	virtual bool OnLinkMoveStarted(UObject* PathComp, const FVector& DestPoint) override;
	
	void SetRelativeStartAndEnd(const FVector& Left, const FVector& Right, AActor* Proxy);

public:
	UPROPERTY(EditAnywhere, Category="Jump")
	float JumpPower = 0.f;

	UPROPERTY(EditAnywhere, Category="Jump")
	float JumpOffsetUp = 100.f;

	UPROPERTY(EditAnywhere, Category="Jump")
	float JumpOffsetDown = 100.f;
};
