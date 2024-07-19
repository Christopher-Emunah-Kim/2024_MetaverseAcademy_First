// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API UPlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	//플레이어 이동속도
	UPROPERTY(EditdefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float speed = 0;

	//플레이어가 공중에 있는지 여부 체크
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool isInAir = false;
};
