// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCheatHelper.h"

#include "Character/Components/BaseStatusComponent.h"

void UCharacterCheatHelper::SetInvincibleMode(const bool bInvincibleMode) const
{
	if (!GetWorld()) return;

	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	const APawn* Pawn = PC->GetPawn();
	if (!Pawn) return;

	UBaseStatusComponent* Status=Pawn->FindComponentByClass<UBaseStatusComponent>();
	if (Status)
	{
		Status->SetInvincibleMode(bInvincibleMode);
	}
}
