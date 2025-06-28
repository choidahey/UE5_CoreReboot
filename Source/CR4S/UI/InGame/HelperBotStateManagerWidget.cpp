#include "HelperBotStateManagerWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../FriendlyAI/Controller/HelperBotAIController.h"
#include "../../FriendlyAI/BaseHelperBot.h"
#include "Character/Characters/PlayerCharacter.h"
#include "UI/InGame/SurvivalHUD.h"
#include "Inventory/OpenWidgetType.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Game/System/AudioManager.h"
#include "../../FriendlyAI/Data/HelperBotSoundData.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"

void UHelperBotStateManagerWidget::InitializeWithController(AHelperBotAIController* InController, EHelperBotState InPreviousState)
{
	OwnerAIController = InController;
	if (InController)
	{
		HelperBot = Cast<ABaseHelperBot>(InController->GetPawn());
		PreviousState = InPreviousState;
		
		if (HelperBot)
		{
			if (BotNameText) BotNameText->SetText(HelperBot->GetBotName());
			if (BotNameEditBox) 
			{
				BotNameEditBox->SetText(HelperBot->GetBotName());
				BotNameEditBox->SetIsReadOnly(true);
				BotNameEditBox->SetVisibility(ESlateVisibility::Collapsed);
			}
			if (BotNameText) BotNameText->SetVisibility(ESlateVisibility::Visible);
		}
		
		InController->SetBotState(EHelperBotState::Idle);
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (APawn* Player = PC->GetPawn())
			{
				FVector LookDirection = (Player->GetActorLocation() - HelperBot->GetActorLocation()).GetSafeNormal();
				TargetLookRotation = LookDirection.Rotation();
				TargetLookRotation.Pitch = 0.0f;

				GetWorld()->GetTimerManager().SetTimer(LookAtPlayerTimer, this,
				                                       &UHelperBotStateManagerWidget::UpdateLookAtPlayer, 0.02f, true);
			}
		}
		UpdateHealthDisplay();
		UpdateRepairButtonState();
	}
}


void UHelperBotStateManagerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	if (SetIdleButton)
	{
		SetIdleButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetIdle);
		SetIdleButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}
	if (SetFollowingButton)
	{
		SetFollowingButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetFollowing);
		SetFollowingButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}
	if (SetChopWoodButton)
	{
		SetChopWoodButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetChopWood);
		SetChopWoodButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}
	if (SetGatheringButton)
	{
		SetGatheringButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetGathering);
		SetGatheringButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::CloseStateWidget);
		CloseButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}
	if (OpenInventoryButton)
	{
		OpenInventoryButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OpenInventory);
		OpenInventoryButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}
	if (SetMiningButton)
	{
		SetMiningButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetMining);
		SetMiningButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}
	if (SetRepairingButton)
	{
		SetRepairingButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetRepairing);
		SetRepairingButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}
	if (SetDefendingButton)
	{
		SetDefendingButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetDefending);
		SetDefendingButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}
	if (ChangeNameButton)
	{
		ChangeNameButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::ChangeNameButtonClicked);
		ChangeNameButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}
	if (BotNameEditBox) BotNameEditBox->OnTextCommitted.AddDynamic(
		this, &UHelperBotStateManagerWidget::OnNameEditCommitted);
	if (PickUpButton)
	{
		PickUpButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::PickUp);
		PickUpButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}
	if (RepairBotButton)
	{
		RepairBotButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::RepairBot);
		RepairBotButton->OnHovered.AddDynamic(this, &UHelperBotStateManagerWidget::OnButtonHovered);
	}

		
	GetWorld()->GetTimerManager().SetTimer(DistanceCheckTimer, this, 
		&UHelperBotStateManagerWidget::CheckPlayerDistance, 0.5f, true);

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;
	}
}

void UHelperBotStateManagerWidget::SetIdle()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Idle);

	if (HelperBot && HelperBot->SoundData && HelperBot->SoundData->CommandResponseSound)
	{
		HelperBot->PlayBotSound(HelperBot->SoundData->CommandResponseSound);
	}
    
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetFollowing()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Following);
	
	if (HelperBot && HelperBot->SoundData && HelperBot->SoundData->CommandResponseSound)
	{
		HelperBot->PlayBotSound(HelperBot->SoundData->CommandResponseSound);
	}
    
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetChopWood()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::ChopWood);
	
	if (HelperBot && HelperBot->SoundData && HelperBot->SoundData->CommandResponseSound)
	{
		HelperBot->PlayBotSound(HelperBot->SoundData->CommandResponseSound);
	}
    
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::CloseStateWidget()
{
	CloseWidgetAndResetInput();
}


void UHelperBotStateManagerWidget::OpenInventory()
{
	if (!HelperBot)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
	{
		return;
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PC->GetPawn());
	if (!PlayerCharacter)
	{
		return;
	}

	UPlayerInventoryComponent* PlayerInvComp = PlayerCharacter->FindComponentByClass<UPlayerInventoryComponent>();
	UBaseInventoryComponent* AIInvComp     = HelperBot->FindComponentByClass<UBaseInventoryComponent>();
	if (!PlayerInvComp || !AIInvComp)
	{
		return;
	}
	
	CloseWidgetAndResetInput();
	PlayerInvComp->OpenOtherInventoryWidget(EOpenWidgetType::ItemPouch, AIInvComp);
}

void UHelperBotStateManagerWidget::SetMining()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Mining);
	
	if (HelperBot && HelperBot->SoundData && HelperBot->SoundData->CommandResponseSound)
	{
		HelperBot->PlayBotSound(HelperBot->SoundData->CommandResponseSound);
	}
    
	CloseWidgetAndResetInput();
}


void UHelperBotStateManagerWidget::SetRepairing()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Repairing);

	if (HelperBot && HelperBot->SoundData && HelperBot->SoundData->CommandResponseSound)
	{
		HelperBot->PlayBotSound(HelperBot->SoundData->CommandResponseSound);
	}
    
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetGathering()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Gathering);
	
	if (HelperBot && HelperBot->SoundData && HelperBot->SoundData->CommandResponseSound)
	{
		HelperBot->PlayBotSound(HelperBot->SoundData->CommandResponseSound);
	}
    
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetDefending()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Defending);
	
	if (HelperBot && HelperBot->SoundData && HelperBot->SoundData->CommandResponseSound)
	{
		HelperBot->PlayBotSound(HelperBot->SoundData->CommandResponseSound);
	}
    
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::CloseWidgetAndResetInput()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DistanceCheckTimer);
		GetWorld()->GetTimerManager().ClearTimer(LookAtPlayerTimer);
	}
	
	if (ASurvivalHUD* HUD = Cast<ASurvivalHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()))
	{
		HUD->ToggleWidget(this);
		HUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
	}
}

void UHelperBotStateManagerWidget::CloseWidgetAndRestorePreviousState()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DistanceCheckTimer);
		GetWorld()->GetTimerManager().ClearTimer(LookAtPlayerTimer);
	}
	if (OwnerAIController)
	{
		OwnerAIController->SetBotState(PreviousState);
	}
	
	if (ASurvivalHUD* HUD = Cast<ASurvivalHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()))
	{
		HUD->ToggleWidget(this);
		HUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
	}
}

void UHelperBotStateManagerWidget::CheckPlayerDistance()
{
	if (!HelperBot)
	{
		return;
	}
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC || !PC->GetPawn())
	{
		return;
	}
	
	float Distance = FVector::Dist(HelperBot->GetActorLocation(), PC->GetPawn()->GetActorLocation());
	
	if (Distance > MaxInteractionDistance)
	{
		CloseWidgetAndRestorePreviousState();
	}
}

void UHelperBotStateManagerWidget::UpdateLookAtPlayer()
{
	if (!HelperBot) return;
    
	FRotator CurrentRotation = HelperBot->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetLookRotation, 0.02f, 3.0f);
	HelperBot->SetActorRotation(NewRotation);
    
	if (FMath::IsNearlyEqual(CurrentRotation.Yaw, TargetLookRotation.Yaw, 1.0f))
	{
		GetWorld()->GetTimerManager().ClearTimer(LookAtPlayerTimer);
	}
}

void UHelperBotStateManagerWidget::ChangeNameButtonClicked()
{
	if (!HelperBot || !BotNameText || !BotNameEditBox || !ChangeNameButton) return;

	if (!bIsEditingName)
	{
		bIsEditingName = true;
		BotNameText->SetVisibility(ESlateVisibility::Collapsed);
		BotNameEditBox->SetVisibility(ESlateVisibility::Visible);
		BotNameEditBox->SetIsReadOnly(false);
		BotNameEditBox->SetKeyboardFocus();
	}
	else
	{
		FText NewName = BotNameEditBox->GetText();
		FString NameString = NewName.ToString();

		if (NameString.Len() >= 2 && NameString.Len() <= 8)
		{
			HelperBot->SetBotName(NewName);
			BotNameText->SetText(NewName);
			
			bIsEditingName = false;
			BotNameText->SetVisibility(ESlateVisibility::Visible);
			BotNameEditBox->SetVisibility(ESlateVisibility::Collapsed);
			BotNameEditBox->SetIsReadOnly(true);
		}
		else
		{
			BotNameEditBox->SetText(HelperBot->GetBotName());
		}
	}
}

void UHelperBotStateManagerWidget::OnNameEditCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter && bIsEditingName)
	{
		ChangeNameButtonClicked();
	}
}


void UHelperBotStateManagerWidget::PickUp()
{
	if (OwnerAIController)
	{
		if (ABaseHelperBot* Bot = Cast<ABaseHelperBot>(OwnerAIController->GetPawn()))
		{
			if (APlayerCharacter* PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0)))
			{
				if (UBaseInventoryComponent* BotInventory = Bot->FindComponentByClass<UBaseInventoryComponent>())
				{
					if (UPlayerInventoryComponent* PlayerInventory = PC->FindComponentByClass<UPlayerInventoryComponent>())
					{
						TMap<FName, int32> ItemsToTransfer;
						const TArray<TObjectPtr<UBaseInventoryItem>>& BotItems = BotInventory->GetInventoryItems();
						
						for (const UBaseInventoryItem* Item : BotItems)
						{
							if (IsValid(Item))
							{
								FName ItemRowName = Item->GetInventoryItemData()->RowName;
								int32 ItemCount = Item->GetCurrentStackCount();
								ItemsToTransfer.FindOrAdd(ItemRowName) += ItemCount;
							}
						}
						
						PlayerInventory->AddItems(ItemsToTransfer);
					}
				}
				
				if (Bot->SoundData && Bot->SoundData->RecallSound)
				{
					Bot->PlayBotSound(Bot->SoundData->RecallSound);
				}

				Bot->StartFadeOut();
				if (UBaseInventoryComponent* InvComp = PC->FindComponentByClass<UBaseInventoryComponent>())
				{
					FHelperPickUpData PickUpData;
					PickUpData.BotName = Bot->GetBotName();
					PickUpData.CurrentHealth = Bot->GetCurrentHealth();
					InvComp->AddHelperBotItem(TEXT("HelperBot"), 1, PickUpData);
				}
			}
		}
	}
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::RepairBot()
{
	if (!HelperBot)
	{
		return;
	}
	
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!Player)
	{
		return;
	}
	
	if (HelperBot->RepairBot(Player))
	{
		UpdateHealthDisplay();
		UpdateRepairButtonState();

		if (HelperBot->SoundData && HelperBot->SoundData->SelfRepairUISound)
		{
			PlayUISound(HelperBot->SoundData->SelfRepairUISound);
		}
	}
}

void UHelperBotStateManagerWidget::UpdateRepairButtonState()
{
	if (!RepairBotButton || !HelperBot)
	{
		return;
	}
	
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	bool bCanRepair = HelperBot->CanRepair(Player);
	
	RepairBotButton->SetIsEnabled(bCanRepair);
}

void UHelperBotStateManagerWidget::UpdateHealthDisplay()
{
	if (!HelperBot)
	{
		return;
	}
   
	if (HealthText)
	{
		FString HealthString = FString::Printf(TEXT("%d / %d"), 
			(int32)HelperBot->GetCurrentHealth(), 
			(int32)HelperBot->GetMaxHealth());
		HealthText->SetText(FText::FromString(HealthString));
	}
}

void UHelperBotStateManagerWidget::OnButtonHovered()
{
	if (HelperBot && HelperBot->SoundData && HelperBot->SoundData->InteractButtonHoverSound)
	{
		PlayUISound(HelperBot->SoundData->InteractButtonHoverSound);
	}
}

void UHelperBotStateManagerWidget::PlayUISound(USoundBase* Sound) const
{
	if (!Sound || !HelperBot)
		return;

	const UGameInstance* GameInstance = GetGameInstance();
	if (IsValid(GameInstance))
	{
		UAudioManager* AudioManager = GameInstance->GetSubsystem<UAudioManager>();
		if (IsValid(AudioManager))
		{
			AudioManager->PlaySFX(Sound, FVector::ZeroVector, EConcurrencyType::UI);
		}
	}
}