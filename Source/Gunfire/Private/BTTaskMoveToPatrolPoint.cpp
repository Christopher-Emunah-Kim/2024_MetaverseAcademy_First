// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskMoveToPatrolPoint.h"
#include <EnemyAIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "NavigationSystem.h"

UBTTaskMoveToPatrolPoint::UBTTaskMoveToPatrolPoint()
{
		NodeName = TEXT("MoveToPatrolPoint");
}

EBTNodeResult::Type UBTTaskMoveToPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	
	// 적의 Pawn을 가져옴
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (nullptr == ControllingPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController is nullptr"));
		return EBTNodeResult::Failed;
	}

	// 네비게이션 매시 시스템을 가져옴
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (nullptr == NavSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("NavSystem is nullptr"));
		return EBTNodeResult::Failed;
	}

	// 적이 생성된 기본 위치를 가져옴
	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AEnemyAIController::HomePosKey);
	FNavLocation NextPatrol;

	if (NavSystem->GetRandomPointInNavigableRadius(Origin, Radius, NextPatrol))
	{
		// 블랙보드의 PatrolPosKey에 NextPatrol의 위치를 저장
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AEnemyAIController::PatrolPosKey, NextPatrol.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
