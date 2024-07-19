// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EnemyBehaviorComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UEnemyBehaviorComponent::UEnemyBehaviorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UEnemyBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UEnemyBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UEnemyBehaviorComponent::IsWaitMode()
{
	return (EEnemyStateType)Blackboard->GetValueAsEnum(StateKey) == EEnemyStateType::Wait;
}

bool UEnemyBehaviorComponent::IsPatrolMode()
{
	return (EEnemyStateType)Blackboard->GetValueAsEnum(StateKey) == EEnemyStateType::Patrol;
}

bool UEnemyBehaviorComponent::IsChaseMode()
{
	return (EEnemyStateType)Blackboard->GetValueAsEnum(StateKey) == EEnemyStateType::Chase;
}

bool UEnemyBehaviorComponent::IsAttackMode()
{
	return (EEnemyStateType)Blackboard->GetValueAsEnum(StateKey) == EEnemyStateType::Attack;
}

bool UEnemyBehaviorComponent::IsDead()
{
	return (EEnemyStateType)Blackboard->GetValueAsEnum(StateKey) == EEnemyStateType::Dead;

}

void UEnemyBehaviorComponent::SetBlackboard(UBlackboardComponent* _Blackboard)
{
	Blackboard = _Blackboard;
}

void UEnemyBehaviorComponent::SetWaitMode()
{

}

void UEnemyBehaviorComponent::SetPatrolMode()
{
}

void UEnemyBehaviorComponent::SetChaseMode()
{
}

void UEnemyBehaviorComponent::SetAttackMode()
{
}

void UEnemyBehaviorComponent::SetDeadMode()
{
}

ACharacter* UEnemyBehaviorComponent::GetPlayerCharacter()
{
	return Cast<ACharacter>(Blackboard->GetValueAsObject(PlayerKey));
}

void UEnemyBehaviorComponent::ChangeType(EEnemyStateType _Type)
{
	EEnemyStateType type = (EEnemyStateType)Blackboard->GetValueAsEnum(StateKey);
	Blackboard->SetValueAsEnum(StateKey, (uint8)_Type);
}

