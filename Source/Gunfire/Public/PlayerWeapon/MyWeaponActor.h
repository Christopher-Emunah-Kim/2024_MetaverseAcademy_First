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
	//�ѱ�� ���� �����Լ� ����
	virtual void WeaponAttack();
	virtual void WeaponReload();
	virtual void WeaponAnim();
	virtual void WeaponStopAttack();

	//�÷��̾� ���ݷ� ���� �����Լ�
	void SetPlayerBuff(float value);

	float AttackPower;

	//�ѱ� ���ݽ� ����Ʈ���̽� ���� ī�޶�����
	void SetCamera(class UCameraComponent* currentCamComp);
	UCameraComponent* CameraComp;

	//�ѱ� ����Ʈ���� ����
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

	//�ѱ� �⺻ ����Ʈ ����
	UPROPERTY(EditAnywhere)
	class UParticleSystem* bulletEffectFactory;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* WeaponTrail;

	UPROPERTY(EditAnywhere)
	FTimerHandle WeaponEffectTimerHandle;

	//��������Ʈ�� ���� ���̾ư���ý��� ���� ����
	UPROPERTY(EditAnywhere, Category = Effects)
	class UNiagaraComponent* WeaponSpawnEffect;

	//Weapon �浹ü �߰�
	UPROPERTY(EditAnywhere)
	class UBoxComponent* WeaponBoxComp;

	//Weapon ���̷�Ż �޽� �߰�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)
	class USkeletalMeshComponent* GunMesh;

	//Weapon SFX �߰�
	//�ѱ� �߻� Sound
	UPROPERTY(EditAnywhere)
	class USoundBase* WeaponShotSound;

	//�ѱ� NoWeaponSound(źâ �������)
	UPROPERTY(EditAnywhere)
	class USoundBase* WeaponNoBulletSound;

	//�ѱ� Reload Sound 
	UPROPERTY(EditAnywhere)
	class USoundBase* WeaponReloadSound;

};
