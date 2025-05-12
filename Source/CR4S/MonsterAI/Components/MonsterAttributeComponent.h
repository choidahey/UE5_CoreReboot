#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CR4S/MonsterAI/Data/MonsterAttributeRow.h"
#include "MonsterAttributeComponent.generated.h"

class UDataTable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CR4S_API UMonsterAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMonsterAttributeComponent();

#pragma region MonsterAttribute
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void InitializeAttribute(const FName MonsterKey);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ApplyDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ApplyHeal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsDead() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	FORCEINLINE float GetCurrentHP() const { return CurrentHP; }


	UPROPERTY(EditAnywhere, Category = "Attributes")
	UDataTable* MonsterAttributeTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	FMonsterAttributeRow CurrentAttribute;

	UPROPERTY(VisibleAnywhere, Category = "Attributes")
	float CurrentHP;
#pragma endregion

protected:
	virtual void BeginPlay() override;

private:
	void ChangeHP(float Delta);

	FString MyHeader;
	
};
