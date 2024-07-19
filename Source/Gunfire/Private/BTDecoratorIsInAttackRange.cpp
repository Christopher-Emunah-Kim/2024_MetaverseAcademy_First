// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecoratorIsInAttackRange.h"
#include "AIController.h"
#include "TPSPlayer.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecoratorIsInAttackRange::UBTDecoratorIsInAttackRange()
{
	NodeName = TEXT("Is In Attack Range");
}

bool UBTDecoratorIsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn == nullptr)
	{
		return false;
	}

	auto Player = Cast<ATPSPlayer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::PlayerCharacterKey));

	bResult = (Player->GetDistanceTo(ControllingPawn) <= 2000.0f);
	UE_LOG(LogTemp, Warning, TEXT("Player Distance: %f"), Player->GetDistanceTo(ControllingPawn));

	// bResult ·Î±×
	return bResult;
}
