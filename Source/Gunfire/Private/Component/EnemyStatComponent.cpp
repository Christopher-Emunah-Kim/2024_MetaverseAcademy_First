// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EnemyStatComponent.h"

// Sets default values for this component's properties
UEnemyStatComponent::UEnemyStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UEnemyStatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay HP : %f MAX HP : %f"), CurrentHP, MaxHP);
}

// Called every frame
void UEnemyStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEnemyStatComponent::SetDamage(float NewDamage)
{
	CurrentHP = FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, MaxHP);
	SetHP(CurrentHP);
}

void UEnemyStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();
	if (CurrentHP < KINDA_SMALL_NUMBER)
	{
		CurrentHP = 0.0f;

		UE_LOG(LogTemp, Warning, TEXT("Enemy HP is Zero"));
		OnHPIsZero.Broadcast();
	}
}

float UEnemyStatComponent::GetHPRatio()
{
	return (MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / MaxHP);
}

bool UEnemyStatComponent::IsDead() const
{
	return CurrentHP <= 0.0f;
}

void UEnemyStatComponent::InitHP(float hp)
{
	MaxHP = hp;
	CurrentHP = MaxHP;
	SetHP(MaxHP);
	UE_LOG(LogTemp, Warning, TEXT("InitHP HP is %f"), CurrentHP);
}

