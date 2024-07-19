// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Gunfire.h"
#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTServiceDetect.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API UBTServiceDetect : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTServiceDetect();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
