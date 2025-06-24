#include "BaseBuildingGimmick.h"

#include "Components/WidgetComponent.h"
#include "Gimmick/UI/BuildingDurabilityWidget.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"


ABaseBuildingGimmick::ABaseBuildingGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);
	
	BuildingDurabilityWidgetComponent
		= CreateDefaultSubobject<UWidgetComponent>(TEXT("BuildingDurabilityWidgetComponent"));
	BuildingDurabilityWidgetComponent->SetupAttachment(RootComponent);

	Tags.Emplace("House");
}

void ABaseBuildingGimmick::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		ItemGimmickSubsystem = World->GetSubsystem<UItemGimmickSubsystem>();
	}

	if (IsValid(BuildingDurabilityWidgetComponent))
	{
		BuildingDurabilityWidgetInstance = Cast<UBuildingDurabilityWidget>(BuildingDurabilityWidgetComponent->GetUserWidgetObject());

		if (IsValid(BuildingDurabilityWidgetInstance))
		{
			BuildingDurabilityWidgetInstance->InitWidget(this);
			ToggleDurabilityWidget(false);
		}
	}
}

void ABaseBuildingGimmick::ToggleDurabilityWidget(const bool bIsOpen)
{
	if (IsValid(BuildingDurabilityWidgetComponent))
	{
		BuildingDurabilityWidgetComponent->SetVisibility(bIsOpen);
	}
}
