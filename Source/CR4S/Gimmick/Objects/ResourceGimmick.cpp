#include "ResourceGimmick.h"

#include "CR4S/Gimmick/Components/DestructibleComponent.h"

AResourceGimmick::AResourceGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("DestructibleComponent"));
}

