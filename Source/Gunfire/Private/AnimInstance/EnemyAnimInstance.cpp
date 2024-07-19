// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/EnemyAnimInstance.h"

UEnemyAnimInstance::UEnemyAnimInstance()
{
	
}

void UEnemyAnimInstance::PlayMontage()
{
	if (IsValid(AttackMontage))
	{
		Montage_Play(AttackMontage, 1.0f);
	}
}

void UEnemyAnimInstance::AnimNotify_Attack()
{
	// �ν��Ͻ� ���� ����
	auto Owner = TryGetPawnOwner();

	// �����ڰ� ������ �α׸� ����Ѵ�.
	if (Owner != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimNotify_Attack Owner : %s"), *Owner->GetName());
	}

	OnAttack.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_Smash()
{
	OnSmash.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_Summon()
{
	OnSummon.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_Laser()
{
	OnLaser.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_ChasingStone()
{
	OnChasingStone.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_PunchReady()
{
	OnPunchReady.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_PunchFireL()
{
	OnPunchFireL.Broadcast();
}

void UEnemyAnimInstance::AnimNotify_PunchFireR()
{
	OnPunchFireR.Broadcast();
}
