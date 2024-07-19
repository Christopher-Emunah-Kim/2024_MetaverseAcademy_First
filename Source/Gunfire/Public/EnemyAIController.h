// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class GUNFIRE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	virtual void BeginPlay() override;

	AEnemyAIController();

	virtual void OnPossess(APawn* InPawn) override;

	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName PlayerCharacterKey;
	static const FName AttackRangeKey;

protected:
	// Called when the game starts or when spawned
	UPROPERTY(EditAnywhere, Category = AI)
	class UBehaviorTree* BehaviorTree;


	UPROPERTY(EditAnywhere, Category = AI)
	float AttackRange = 100.0f;
	//UPROPERTY(EditDefaultsOnly, Category = AI)
	//AIPerceptionComponent* PerceptionComponent;
};
