// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTask_EnemyMoveTo.h"

UBTTask_EnemyMoveTo::UBTTask_EnemyMoveTo()
{
}

EBTNodeResult::Type UBTTask_EnemyMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	return EBTNodeResult::Succeeded;
}
