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
	
	// ���� Pawn�� ������
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (nullptr == ControllingPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIController is nullptr"));
		return EBTNodeResult::Failed;
	}

	// �׺���̼� �Ž� �ý����� ������
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(ControllingPawn->GetWorld());
	if (nullptr == NavSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("NavSystem is nullptr"));
		return EBTNodeResult::Failed;
	}

	// ���� ������ �⺻ ��ġ�� ������
	FVector Origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector(AEnemyAIController::HomePosKey);
	FNavLocation NextPatrol;

	if (NavSystem->GetRandomPointInNavigableRadius(Origin, Radius, NextPatrol))
	{
		// �������� PatrolPosKey�� NextPatrol�� ��ġ�� ����
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(AEnemyAIController::PatrolPosKey, NextPatrol.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
