// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTaskAttack.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API UBTTaskAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTaskAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool IsAttacking = false;
	bool bTaskComplete = false;
};
