// Fill out your copyright notice in the Description page of Project Settings.

#include "BTServiceDetect.h"
#include "EnemyAIController.h"
#include "TPSPlayer.h"
#include "Enemy/Enemy.h"
#include <BehaviorTree/BlackboardComponent.h>
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

UBTServiceDetect::UBTServiceDetect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UBTServiceDetect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	UWorld* World = ControllingPawn->GetWorld();
	FVector Center = ControllingPawn->GetActorLocation();
	float   DetectRadios = 6000.0f;

	if (nullptr == World) return;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);

	bool bResult = World->OverlapMultiByChannel
	(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel12,
		FCollisionShape::MakeSphere(DetectRadios),
		CollisionQueryParam
	);

	if (bResult)
	{
		for (auto const& OverlapResult : OverlapResults)
		{
			ATPSPlayer* Player = Cast<ATPSPlayer>(OverlapResult.GetActor());
			if (Player && Player->GetController()->IsPlayerController())
			{
				UBlackboardComponent* comp = OwnerComp.GetBlackboardComponent();
				comp->SetValueAsObject(AEnemyAIController::PlayerCharacterKey, Player);

				DrawDebugSphere(World, Center, DetectRadios, 16, FColor::Green, false, 0.4f);
				DrawDebugPoint(World, Player->GetActorLocation(), 10.0f, FColor::Blue, false, 0.4f);
				DrawDebugLine(World, ControllingPawn->GetActorLocation(), Player->GetActorLocation(), FColor::Blue, false, 0.4f);
				return;
			}
		}
	}

	DrawDebugSphere(World, Center, DetectRadios, 16, FColor::Red, false, 0.4f);
}
