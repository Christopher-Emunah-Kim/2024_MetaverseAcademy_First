// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService/BTService_IsEnemyDead.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "Component/EnemyBehaviorComponent.h"
#include "Enemy/Enemy.h"

UBTService_IsEnemyDead::UBTService_IsEnemyDead()
{
}

void UBTService_IsEnemyDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    auto EnemyBehavior = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());

    if (EnemyBehavior != nullptr)
    {
        if (EnemyBehavior->IsDead())
        {
            OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
        }
        else
        {
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), true);
		}
    }
}
