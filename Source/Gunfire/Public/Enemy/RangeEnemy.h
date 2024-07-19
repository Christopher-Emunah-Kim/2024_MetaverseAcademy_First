// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "RangeEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API ARangeEnemy : public AEnemy
{
	GENERATED_BODY()
	
	public:
	ARangeEnemy();

	virtual void BeginPlay() override;
		
	virtual void OnAttack() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* WeaponMesh;
};
