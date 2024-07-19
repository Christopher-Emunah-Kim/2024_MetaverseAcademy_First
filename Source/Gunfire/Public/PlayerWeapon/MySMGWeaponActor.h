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
	//총기류 공통 가상함수 선언
	virtual void WeaponAttack() override;
	virtual void WeaponReload() override;
	virtual void WeaponAnim() override;
	virtual void WeaponStopAttack() override;

	//SMG 자동발사타이머핸들
	FTimerHandle SMGFIreTimer;
	//SMG발사속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SMG)
	float SMGFireRate;
	//SMG 발사처리함수(LineTrace사용)
	void HandleSMGFire();

};
