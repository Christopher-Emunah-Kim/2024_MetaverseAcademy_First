// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWeapon/MyShotGunWeaponActor.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h" //�߰�
#include "NiagaraComponent.h" //�߰�
#include "NiagaraFunctionLibrary.h" //�߰�

AMyShotGunWeaponActor::AMyShotGunWeaponActor()
{
	//�浹üũ�⼳��
	WeaponBoxComp->SetBoxExtent(FVector(15.0f));
	//4-2. ���̷�Ż�޽� �����ͷε�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempShotGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MarketplaceBlockout/Modern/Weapons/Assets/Shotguns/02/SKM_Modern_Weapons_Shotgun_02.SKM_Modern_Weapons_Shotgun_02'"));
	//4-3. �����ͷε尡 �����ߴٸ�
	if (TempShotGunMesh.Succeeded())
	{
		//4-4. ���̷�Ż�޽� ������ �Ҵ�
		GunMesh->SetSkeletalMesh(TempShotGunMesh.Object);
	}
}

void AMyShotGunWeaponActor::BeginPlay()
{
	BaseWeaponDamage = 15.0f;
	MaxWeaponAmmo = 10;
	BaseWeaponRange = 2000.0f;
	CurrentWeaponAmmo = MaxWeaponAmmo;
}

void AMyShotGunWeaponActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AMyShotGunWeaponActor::WeaponAttack()
{
	if (CurrentWeaponAmmo > 0)
	{
		HandleShotGunFire();
		CurrentWeaponAmmo--;
		UE_LOG(LogTemp, Warning, TEXT("AMMO : %d"), CurrentWeaponAmmo);
		GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Blue, FString::Printf(TEXT("AMMO : %d"), CurrentWeaponAmmo));

		//�Ѿ� �߻�Ҹ��� �÷����Ѵ�.
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponShotSound);

	}
	else
	{
		//�Ѿ� ��źâ�Ҹ��� �÷����Ѵ�.
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponNoBulletSound);

		//�׳� �ڵ� �������� ���Ѵٸ�..
		//WeaponReload();
	}
}

void AMyShotGunWeaponActor::WeaponReload()
{
	Super::WeaponReload();
}

void AMyShotGunWeaponActor::WeaponAnim()
{

}

void AMyShotGunWeaponActor::WeaponStopAttack()
{

}

void AMyShotGunWeaponActor::HandleShotGunFire()
{
	// ī�޶� ������Ʈ ����
	UCameraComponent* activeCamComp = CameraComp;

	FVector startPos = activeCamComp->GetComponentLocation();
	FVector forwardVector = activeCamComp->GetForwardVector();

	//���������� ��������� ���� ����
	float horizontalAngleStep = 4.0f; //���򰢵� ����
	float verticalAngleStep = 4.0f;   //�������� ����
	int32 numTraces = 8; //����Ʈ���̽� ����

	//10���� ����Ʈ���̽����� ��������� ����(���Ǹ��)
	for (int32 i = 0; i < numTraces; i++)
	{
		//������������ �����Ͽ� �������·� �������� ��
		float randomYaw = FMath::RandRange(-horizontalAngleStep, horizontalAngleStep);
		float randomPitch = FMath::RandRange(-verticalAngleStep, verticalAngleStep);

		//������ ȸ�����ͷ� ��ȯ
		FRotator randomRotator = FRotator(randomPitch, randomYaw, 0.0f);
		FVector rotatedVector = randomRotator.RotateVector(forwardVector);
		FVector endPos = startPos + (rotatedVector * BaseWeaponRange);

		//�浹���� ����
		FHitResult hitInfo;
		FCollisionQueryParams params;
		//�� �ڽ��� �浹 ����
		params.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

		//����Ʈ���̽����� �� ������ ���� �浹 �� ������ ����
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

		// ���̾ư��� �ý��� ����
		UNiagaraComponent* TrailNSC = nullptr;
		if (WeaponTrail)
		{
			FVector TraceVector = hitInfo.ImpactPoint - startPos;
			FVector SpawnPos = startPos + (TraceVector * 0.7f); // 70% �������� ����
			FRotator TrailRotation = TraceVector.Rotation();
			TrailNSC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WeaponTrail, SpawnPos, TrailRotation);
		}

		if (bHit)
		{
			// �浹 ó�� -> �Ѿ� ���� ȿ�� ���
			FTransform bulletTrans;
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			// ������ ���Ϳ� ������ ����
			UGameplayStatics::ApplyPointDamage(hitInfo.GetActor(), BaseWeaponDamage * AttackPower, rotatedVector, hitInfo, UGameplayStatics::GetPlayerPawn(GetWorld(),0)->GetController(), this, nullptr);


			// �ǰ� ��ü ���������� ����
			auto hitComp = hitInfo.GetComponent();
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 3000;
				hitComp->AddForce(force);
			}

			// bHit�� ������ 0.2�� ������ ���̾ư��� �ý��� ��Ȱ��ȭ
			if (TrailNSC)
			{
				GetWorld()->GetTimerManager().SetTimer(WeaponEffectTimerHandle, [TrailNSC]()
					{
						if (TrailNSC)
						{
							TrailNSC->Deactivate();
						}
					}, 0.2f, false);
			}
		}
		//�߻���� ����
		bIsFiring = true;

		// �ѱ� ȭ�� ��ƼŬ ����
		if (MuzzleFlash)
		{
			FTransform MuzzleTransform = GunMesh->GetSocketTransform(TEXT("FirePosition"));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
		};
	}
}
