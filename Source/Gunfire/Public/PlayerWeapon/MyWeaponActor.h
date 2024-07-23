// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyWeaponActor.generated.h"
class ATPSPlayer;
UCLASS()
class GUNFIRE_API AMyWeaponActor : public AActor 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//총기류 공통 가상함수 선언
	virtual void WeaponAttack();
	virtual void WeaponReload();
	virtual void WeaponAnim();
	virtual void WeaponStopAttack();

	//플레이어 공격력 스탯 적용함수
	void SetPlayerBuff(float value);

	float AttackPower;

	//총기 공격시 라인트레이스 기준 카메라정보
	void SetCamera(class UCameraComponent* currentCamComp);
	UCameraComponent* CameraComp;

	//총기 디폴트설정 선언
	UPROPERTY(EditAnywhere)
	float BaseWeaponDamage;

	UPROPERTY(EditAnywhere)
	float BaseWeaponRange;

	UPROPERTY(EditAnywhere)
	int32 MaxWeaponAmmo;

	UPROPERTY(EditAnywhere)
	int32 CurrentWeaponAmmo;

	UPROPERTY(EditAnywhere)
	FTimerHandle ReloadTimerHandle;

	bool bIsFiring;
	UPROPERTY(EditAnywhere)
	bool bSniperMode;

	//총기 기본 이펙트 선언
	UPROPERTY(EditAnywhere)
	class UParticleSystem* bulletEffectFactory;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* WeaponTrail;

	UPROPERTY(EditAnywhere)
	FTimerHandle WeaponEffectTimerHandle;

	//스폰이펙트를 위한 나이아가라시스템 변수 선언
	UPROPERTY(EditAnywhere, Category = Effects)
	class UNiagaraComponent* WeaponSpawnEffect;

	//Weapon 충돌체 추가
	UPROPERTY(EditAnywhere)
	class UBoxComponent* WeaponBoxComp;

	//Weapon 스켈레탈 메시 추가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)
	class USkeletalMeshComponent* GunMesh;

	//Weapon SFX 추가
	//총기 발사 Sound
	UPROPERTY(EditAnywhere)
	class USoundBase* WeaponShotSound;

	//총기 NoWeaponSound(탄창 비었을때)
	UPROPERTY(EditAnywhere)
	class USoundBase* WeaponNoBulletSound;

	//총기 Reload Sound 
	UPROPERTY(EditAnywhere)
	class USoundBase* WeaponReloadSound;

};
