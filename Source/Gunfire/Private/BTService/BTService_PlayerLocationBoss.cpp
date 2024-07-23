// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService/BTService_PlayerLocationBoss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "Component/EnemyBehaviorComponent.h"
#include "Enemy/Enemy.h"

UBTService_PlayerLocationBoss::UBTService_PlayerLocationBoss()
{
    NodeName = "Update Player Location Boss";
}

void UBTService_PlayerLocationBoss::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (PlayerPawn == nullptr)
    {
        return;
    }

    if (OwnerComp.GetAIOwner() == nullptr)
    {
        return;
    }

    bool bAttacking = OwnerComp.GetBlackboardComponent()->GetValueAsBool(TEXT("bAttacking"));
    if (bAttacking)
    {
        return;
    }

    // 플레이어 폰이 지정되었다면 리턴한다.
    auto* targeted = OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey());
    if (targeted != nullptr)
    {
        return;
    }

    if (OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn))
    {
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn);
    }
    else
    {
        OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
    }
}
