// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_IsEnemyDead.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API UBTService_IsEnemyDead : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_IsEnemyDead();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
