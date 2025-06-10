#include "UI/InGame/WorldMapWidget.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UWorldMapWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWorldMapWidget::UpdateWorldMap()
{
	FindPlayerLocation();
	// Add Additional Map Features
}

void UWorldMapWidget::FindPlayerLocation()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		APawn* Player = PC->GetPawn();
		if (!Player || !PlayerIcon || !MapImage) return;

		FVector PlayerWorldPos = Player->GetActorLocation();
		FVector2D MapPos = WorldToMapPosition(PlayerWorldPos);

		if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(PlayerIcon->Slot))
		{
			OverlaySlot->SetPadding(FMargin(MapPos.X, MapPos.Y, 0.f, 0.f));
		}

		float PlayerYaw = Player->GetActorRotation().Yaw;
		float IconRotation = PlayerYaw + 90.0f;
		PlayerIcon->SetRenderTransformAngle(IconRotation);

		// 디버그 로그
		FVector2D MapSize = MapImage->Brush.ImageSize;
		float XPercent = (PlayerWorldPos.X - WorldMin.X) / (WorldMax.X - WorldMin.X);
		float YPercent = (PlayerWorldPos.Y - WorldMin.Y) / (WorldMax.Y - WorldMin.Y);

		UE_LOG(LogTemp, Log, TEXT("PlayerWorldPos: %s"), *PlayerWorldPos.ToString());
		UE_LOG(LogTemp, Log, TEXT("MapSize: %s"), *MapSize.ToString());
		UE_LOG(LogTemp, Log, TEXT("XPercent: %.4f, YPercent: %.4f"), XPercent, YPercent);
		UE_LOG(LogTemp, Log, TEXT("MapPos: %s"), *MapPos.ToString());
	}
}

FVector2D UWorldMapWidget::WorldToMapPosition(const FVector& WorldLocation) const
{
	if (!MapImage) return FVector2D::ZeroVector;

	FVector2D MapSize = MapImage->Brush.ImageSize;

	float XPercent = (WorldLocation.X - WorldMin.X) / (WorldMax.X - WorldMin.X);
	float YPercent = (WorldLocation.Y - WorldMin.Y) / (WorldMax.Y - WorldMin.Y);

	return FVector2D(MapSize.X * XPercent, MapSize.Y * YPercent);
}
