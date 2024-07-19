// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerWeapon/MyWeaponActor.h"
#include "MySMGWeaponActor.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API AMySMGWeaponActor : public AMyWeaponActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AMySMGWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//�ѱ�� ���� �����Լ� ����
	virtual void WeaponAttack() override;
	virtual void WeaponReload() override;
	virtual void WeaponAnim() override;
	virtual void WeaponStopAttack() override;

	//SMG �ڵ��߻�Ÿ�̸��ڵ�
	FTimerHandle SMGFIreTimer;
	//SMG�߻�ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SMG)
	float SMGFireRate;
	//SMG �߻�ó���Լ�(LineTrace���)
	void HandleSMGFire();

};
