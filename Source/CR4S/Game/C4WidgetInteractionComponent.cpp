#include "Game/C4WidgetInteractionComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"

void UC4WidgetInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    FVector2D ViewportSize;
    GEngine->GameViewport->GetViewportSize(ViewportSize);

    FVector2D ScreenCenter(ViewportSize.X / 2, ViewportSize.Y / 2);
    FVector WorldOrigin, WorldDirection;

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC && PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldOrigin, WorldDirection))
    {
        FHitResult HitResult;
        FVector TraceEnd = WorldOrigin + WorldDirection * 10000.f;

        if (GetWorld()->LineTraceSingleByChannel(HitResult, WorldOrigin, TraceEnd, ECC_Visibility))
        {
            if (HitResult.Component.IsValid() && HitResult.Component->IsA(UWidgetComponent::StaticClass()))
            {
                SetCustomHitResult(HitResult);

                if (PC->WasInputKeyJustPressed(EKeys::F) || PC->WasInputKeyJustPressed(EKeys::LeftMouseButton))
                {
                    PressPointerKey(EKeys::LeftMouseButton);
                }
                if (PC->WasInputKeyJustReleased(EKeys::F) || PC->WasInputKeyJustReleased(EKeys::LeftMouseButton))
                {
                    ReleasePointerKey(EKeys::LeftMouseButton);
                }
            }
        }
    }
}

