#include "ObjectPoolable.h"

void AObjectPoolable::OnSpawnFromPool()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void AObjectPoolable::OnReturnToPool()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}
