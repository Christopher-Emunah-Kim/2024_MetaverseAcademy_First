// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

// 

const FName AEnemyAIController::HomePosKey(TEXT("HomePos"));
const FName AEnemyAIController::PatrolPosKey(TEXT("PatrolPos"));
const FName AEnemyAIController::PlayerCharacterKey(TEXT("PlayerCharacter"));
const FName AEnemyAIController::AttackRangeKey(TEXT("AttackRange"));

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree != nullptr)
	{
		RunBehaviorTree(BehaviorTree);
		
		UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BehaviorTree is not loaded"));
	
	}
}

AEnemyAIController::AEnemyAIController()
{
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (BehaviorTree != nullptr)
	{
		UBlackboardData* data = BehaviorTree->GetBlackboardAsset();

		UBlackboardComponent* BlackboardComponent;
		if (UseBlackboard(data, BlackboardComponent))
		{
			BlackboardComponent->SetValueAsVector(HomePosKey, InPawn->GetActorLocation());

			BlackboardComponent->SetValueAsFloat(AttackRangeKey, AttackRange);
		}
	}
}
