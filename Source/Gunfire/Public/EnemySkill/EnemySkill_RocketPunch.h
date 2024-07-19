// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySkill_RocketPunch.generated.h"

UCLASS()
class GUNFIRE_API UEnemySkill_RocketPunch : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UEnemySkill_RocketPunch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};
