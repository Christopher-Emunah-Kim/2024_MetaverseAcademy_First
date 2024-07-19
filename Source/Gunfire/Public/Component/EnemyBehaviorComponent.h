// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyBehaviorComponent.generated.h"


UENUM(BlueprintType)
enum class EEnemyStateType : uint8
{
	Wait, Patrol, Chase, Attack, Dead
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UEnemyBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyBehaviorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintPure)
	bool IsWaitMode();

	UFUNCTION(BlueprintPure)
	bool IsPatrolMode();

	UFUNCTION(BlueprintPure)
	bool IsChaseMode();

	UFUNCTION(BlueprintPure)
	bool IsAttackMode();
	
	UFUNCTION(BlueprintPure)
	bool IsDead();


	void SetBlackboard(class UBlackboardComponent* _Blackboard);

public:

	void SetWaitMode();
	void SetPatrolMode();
	void SetChaseMode();
	void SetAttackMode();
	void SetDeadMode();

	ACharacter* GetPlayerCharacter();

private:
	void ChangeType(EEnemyStateType _Type);
	class UBlackboardComponent* Blackboard;

	UPROPERTY(EditAnywhere)
	FName StateKey = "State";

	UPROPERTY(EditAnywhere)
	FName PlayerKey = "PlayerCharacter";

};
