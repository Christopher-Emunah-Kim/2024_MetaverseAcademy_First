// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySkill/EnemySkill_RockPillar.h"

// Sets default values
AEnemySkill_RockPillar::AEnemySkill_RockPillar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

// Called when the game starts or when spawned
void AEnemySkill_RockPillar::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemySkill_RockPillar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySkill_RockPillar::OnExplosion()
{
	DoExplosion();
}


