// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskMoveToPatrolPoint.generated.h"

UCLASS()
class GUNFIRE_API UBTTaskMoveToPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskMoveToPatrolPoint();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	float Radius = 500.0f;
};
