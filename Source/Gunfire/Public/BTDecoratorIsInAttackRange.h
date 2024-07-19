// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecoratorIsInAttackRange.generated.h"

UCLASS()
class GUNFIRE_API UBTDecoratorIsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecoratorIsInAttackRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
