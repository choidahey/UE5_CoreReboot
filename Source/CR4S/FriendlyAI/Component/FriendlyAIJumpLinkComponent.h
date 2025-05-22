#pragma once

#include "CoreMinimal.h"
#include "NavLinkCustomComponent.h"
#include "FriendlyAIJumpLinkComponent.generated.h"

UCLASS(ClassGroup = (Navigation), meta = (BlueprintSpawnableComponent))
class CR4S_API UFriendlyAIJumpLinkComponent : public UNavLinkCustomComponent
{
	GENERATED_BODY()

public:
	UFriendlyAIJumpLinkComponent();
	
	UPROPERTY(EditAnywhere, Category = "Navigation")
	float JumpHeight;
	
	UFUNCTION()
	void OnSmartLinkReached(UNavLinkCustomComponent* ThisComp, UObject* PathComp, const FVector& DestPoint);
	
protected:
	virtual void OnRegister() override;
		
};
