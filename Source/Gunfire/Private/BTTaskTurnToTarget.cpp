// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskTurnToTarget.h"
#include "EnemyAIController.h"
#include "Enemy/Enemy.h"
#include "TPSPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTaskTurnToTarget::UBTTaskTurnToTarget()
{
	NodeName = TEXT("TurnToTarget");
}

EBTNodeResult::Type UBTTaskTurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy)
		return EBTNodeResult::Failed;

	auto Player = Cast<ATPSPlayer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyAIController::PlayerCharacterKey));
	if (nullptr == Player)
		return EBTNodeResult::Failed;

	FVector LookVector = Player->GetActorLocation() - Enemy->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	Enemy->SetActorRotation(FMath::RInterpTo(Enemy->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f));

	return EBTNodeResult::Succeeded;
}
