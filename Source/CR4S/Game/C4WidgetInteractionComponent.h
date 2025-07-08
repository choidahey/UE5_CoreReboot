#pragma once

#include "Components/WidgetInteractionComponent.h"
#include "C4WidgetInteractionComponent.generated.h"

UCLASS()
class CR4S_API UC4WidgetInteractionComponent : public UWidgetInteractionComponent
{
	GENERATED_BODY()

protected:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
};
