// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRangeAttackDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSmashDelegate);
DECLARE_MULTICAST_DELEGATE(FOnSummonDelegate);
DECLARE_MULTICAST_DELEGATE(FOnLaserDelegate);
DECLARE_MULTICAST_DELEGATE(FOnChasingStoneDelegate);
DECLARE_MULTICAST_DELEGATE(FOnPunchReadyDelegate);
DECLARE_MULTICAST_DELEGATE(FOnPunchFireLeftDelegate);
DECLARE_MULTICAST_DELEGATE(FOnPunchFireRightDelegate);
/**
 * 
 */
UCLASS()
class GUNFIRE_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UEnemyAnimInstance();

private:

public:
	void PlayMontage();
public:
	FOnRangeAttackDelegate OnAttack;
	FOnSmashDelegate OnSmash;
	FOnSummonDelegate OnSummon;
	FOnLaserDelegate OnLaser;
	FOnChasingStoneDelegate OnChasingStone;
	FOnPunchReadyDelegate OnPunchReady;
	FOnPunchFireLeftDelegate OnPunchFireL;
	FOnPunchFireRightDelegate OnPunchFireR;

	UPROPERTY(EditAnywhere, Category = Attack, Meta = (AllowPrivateAccess = true));
	UAnimMontage* AttackMontage;
private:
	UFUNCTION()
	void AnimNotify_Attack();
	UFUNCTION()
	void AnimNotify_Smash();	
	UFUNCTION()
	void AnimNotify_Summon();
	UFUNCTION()
	void AnimNotify_Laser();
	UFUNCTION()
	void AnimNotify_ChasingStone();
	UFUNCTION()
	void AnimNotify_PunchReady();
	UFUNCTION()
	void AnimNotify_PunchFireL();
	UFUNCTION()
	void AnimNotify_PunchFireR();
};
