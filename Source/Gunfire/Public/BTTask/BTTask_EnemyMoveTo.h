// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_EnemyMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API UBTTask_EnemyMoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()
	
public:
	UBTTask_EnemyMoveTo();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
