#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseMonster.generated.h"

class UMonsterAttributeComponent;
class UMonsterSkillComponent;
class UMonsterStateComponent;
class UMonsterPerceptionComponent;
class UMonsterAnimComponent;
class UBehaviorTree;

UCLASS()
class CR4S_API ABaseMonster : public ACharacter
{
	GENERATED_BODY()


#pragma region ACharacter Override
public:
	ABaseMonster();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

#pragma endregion

//#pragma region BaseMonster Components
//public:
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
//	UMonsterAttributeComponent* AttributeComp;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
//	UMonsterSkillComponent* SkillComp;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
//	UMonsterStateComponent* StateComp;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
//	UMonsterPerceptionComponent* PerceptionComp;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
//	UMonsterAnimComponent* AnimComp;
//
//#pragma endregion

#pragma region Monster AI
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|AI")
	UBehaviorTree* BehaviorTree;

#pragma endregion

};
