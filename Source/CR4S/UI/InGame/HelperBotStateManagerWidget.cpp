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

	if (SetIdleButton) SetIdleButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetIdle);
	if (SetFollowingButton) SetFollowingButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetFollowing);
	if (SetChopWoodButton) SetChopWoodButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetChopWood);
	if (SetGatheringButton) SetGatheringButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetGathering);
	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::CloseStateWidget);
	if (OpenInventoryButton) OpenInventoryButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OpenInventory);
	if (SetMiningButton) SetMiningButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetMining);
	if (SetRepairingButton) SetRepairingButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetRepairing);
	if (SetDefendingButton) SetDefendingButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetDefending);
	if (ChangeNameButton) ChangeNameButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::ChangeNameButtonClicked);
	if (BotNameEditBox) BotNameEditBox->OnTextCommitted.AddDynamic(this, &UHelperBotStateManagerWidget::OnNameEditCommitted);
	if (PickUpButton) PickUpButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::PickUp);
	if (RepairBotButton) RepairBotButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::RepairBot);

		
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
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetFollowing()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Following);
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetChopWood()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::ChopWood);
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
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetRepairing()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Repairing);
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetGathering()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Gathering);
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetDefending()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Defending);
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