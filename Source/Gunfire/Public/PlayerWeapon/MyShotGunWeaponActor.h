// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerWeapon/MyWeaponActor.h"
#include "MyShotGunWeaponActor.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API AMyShotGunWeaponActor : public AMyWeaponActor
{
	GENERATED_BODY()
	

public:
	// Sets default values for this actor's properties
	AMyShotGunWeaponActor();

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

	
	//ShotGun �߻�ó�� �Լ�
	void HandleShotGunFire();
	

};
