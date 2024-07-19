// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskAttack.h"
#include "Enemy/Enemy.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Component/EnemyBehaviorComponent.h"

UBTTaskAttack::UBTTaskAttack()
{
	bNotifyTick = true;
	IsAttacking = false;
}

EBTNodeResult::Type UBTTaskAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr)
		return EBTNodeResult::Failed;
	
	UE_LOG(LogTemp, Warning, TEXT("ExecuteTask %s"), *Enemy->GetName());

	IsAttacking = true;
	bTaskComplete = false;

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);

	Enemy->OnAttackEnd.AddLambda([this]() -> void
		{
			IsAttacking = false;
		});

	Enemy->DoAttack();
	return EBTNodeResult::InProgress;
}

void UBTTaskAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (!IsAttacking)
	{
		// 비헤이비어 트리 액터
		auto EnemyBehavior = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

		// 로그 출력
		if (EnemyBehavior != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Attack Task Complete %s"), *EnemyBehavior->GetName());
		}

		bTaskComplete = true;
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
