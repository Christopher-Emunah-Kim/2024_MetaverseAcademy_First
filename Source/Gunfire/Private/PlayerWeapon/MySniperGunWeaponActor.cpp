// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWeapon/MySniperGunWeaponActor.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h" //�߰�
#include "NiagaraComponent.h" //�߰�
#include "NiagaraFunctionLibrary.h" //�߰�

AMySniperGunWeaponActor::AMySniperGunWeaponActor()
{
	//�浹üũ�⼳��
	WeaponBoxComp->SetBoxExtent(FVector(15.0f));
	//2-2. ���̷�Ż�޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempSniperMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MarketplaceBlockout/Modern/Weapons/Assets/Snipers/02/SKM_Modern_Weapons_Sniper_02.SKM_Modern_Weapons_Sniper_02'"));
	//2-3. �����ͷε尡 �����ߴٸ�
	if (TempSniperMesh.Succeeded())
	{
		//2-4. �ε��� ����ƽ�޽� ������ �Ҵ�
		GunMesh->SetSkeletalMesh(TempSniperMesh.Object);
	}
}

void AMySniperGunWeaponActor::BeginPlay()
{
	BaseWeaponDamage = 100.0f;
	MaxWeaponAmmo = 7;
	BaseWeaponRange = 8000.0f;
	CurrentWeaponAmmo = MaxWeaponAmmo;
}

void AMySniperGunWeaponActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AMySniperGunWeaponActor::WeaponAttack()
{
	if (CurrentWeaponAmmo > 0)
	{
		HandleSniperFire();
		CurrentWeaponAmmo--;
		UE_LOG(LogTemp, Warning, TEXT("AMMO : %d"), CurrentWeaponAmmo);
		GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Blue, FString::Printf(TEXT("AMMO : %d"), CurrentWeaponAmmo));

		if (CurrentWeaponAmmo == 0)
		{
			WeaponReload();
		}
	}
}

void AMySniperGunWeaponActor::WeaponReload()
{
	Super::WeaponReload();
}

void AMySniperGunWeaponActor::WeaponAnim()
{
	
}

void AMySniperGunWeaponActor::WeaponStopAttack()
{
}

void AMySniperGunWeaponActor::HandleSniperFire()
{
	// ī�޶� ������Ʈ ����
	UCameraComponent* activeCamComp = CameraComp;

	//LineTrace�� ������ġ(Ȱ��ȭ�� ī�޶� ����)
	FVector startPos = activeCamComp->GetComponentLocation();
	//LineTrace�� ������ġ(Ȱ��ȭ�� ī�޶� ���߾ӿ��� �������� N��ŭ)
	FVector endPos = startPos + (activeCamComp->GetForwardVector() * BaseWeaponRange);
	//LineTrace�� �浹������ ���� ����
	FHitResult hitInfo;
	//�浹�ɼ� ��������
	FCollisionQueryParams params;
	//�� �ڽ��� �浹 ����
	params.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	//Channel ���͸� �̿��� LineTrace �浹 ����
	//(�浹����, ������ġ, ������ġ, ����ä��, �浹�ɼ�)
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

	// ���̾ư��� �ý��� ����
	UNiagaraComponent* TrailNSC = nullptr;
	if (WeaponTrail)
	{
		FVector TraceVector = hitInfo.ImpactPoint - startPos;
		FVector SpawnPos = startPos + (TraceVector * 0.1f); // 10% �������� ����
		FRotator TrailRotation = TraceVector.Rotation();
		TrailNSC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WeaponTrail, SpawnPos, TrailRotation);
	}

	//LineTrace�� �ε�������(bHit�� true�� ��)
	if (bHit)
	{
		//�浹ó�� -> �Ѿ� ����ȿ�� ���
		//�Ѿ�����ȿ�� Ʈ������ ���� ����
		FTransform bulletTrans;
		//�ε�ģ ��ġ �Ҵ�
		bulletTrans.SetLocation(hitInfo.ImpactPoint);
		//�Ѿ� ���� ȿ�� �ν��Ͻ� ����
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

		//�ǰݵ� ���Ϳ� ������ ����
		UGameplayStatics::ApplyPointDamage(hitInfo.GetActor(), BaseWeaponDamage * AttackPower, activeCamComp->GetForwardVector(), hitInfo, UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetController(), this, nullptr);

		//�ǰݹ�ü ���������� ����
		auto hitComp = hitInfo.GetComponent();
		//1. ���� ���� ������Ʈ�� ������ ����Ǿ��ִٸ�
		if (hitComp && hitComp->IsSimulatingPhysics())
		{
			//2. �������� ���� ������ ������ �����ְ�
			FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 50000;
			//3. �� �������� ����������.
			hitComp->AddForce(force);
		}

		// bHit�� ������ 0.4�� ������ ���̾ư��� �ý��� ��Ȱ��ȭ
		if (TrailNSC)
		{
			GetWorld()->GetTimerManager().SetTimer(WeaponEffectTimerHandle, [TrailNSC]()
				{
					if (TrailNSC)
					{
						TrailNSC->Deactivate();
					}
				}, 0.4f, false);
		}
	}
	else
	{

	}

	// �ѱ� ȭ�� ��ƼŬ ����
	if (MuzzleFlash)
	{
		FTransform MuzzleTransform = GunMesh->GetSocketTransform(TEXT("FirePosition"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
	}
}
