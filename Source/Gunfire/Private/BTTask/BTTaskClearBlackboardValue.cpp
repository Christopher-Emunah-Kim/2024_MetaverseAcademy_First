// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask/BTTaskClearBlackboardValue.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTaskClearBlackboardValue::UBTTaskClearBlackboardValue()
{
	NodeName = "Clear Blackboard Value";
}

EBTNodeResult::Type UBTTaskClearBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());

	return EBTNodeResult::Succeeded;
}
