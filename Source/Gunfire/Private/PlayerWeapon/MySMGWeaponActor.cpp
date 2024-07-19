// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWeapon/MySMGWeaponActor.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h" //�߰�
#include "NiagaraComponent.h" //�߰�
#include "NiagaraFunctionLibrary.h" //�߰�

AMySMGWeaponActor::AMySMGWeaponActor()
{
	//�浹üũ�⼳��
	WeaponBoxComp->SetBoxExtent(FVector(15.0f));
	//3-2. ���̷�Ż�޽� �����ͷε�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempSMGMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MarketplaceBlockout/Modern/Weapons/Assets/SMGs/01/SKM_Modern_Weapons_SMG_01.SKM_Modern_Weapons_SMG_01'"));
	//3-3. �����ͷε尡 �����ߴٸ�
	if (TempSMGMesh.Succeeded())
	{
		//3-4. ���̷�Ż�޽� ������ �Ҵ�
		GunMesh->SetSkeletalMesh(TempSMGMesh.Object);
	}

}

void AMySMGWeaponActor::BeginPlay()
{
	//SMG�⺻����
	SMGFireRate = 0.1f;  //1�ʿ� 10��
	BaseWeaponDamage = 15.0f;
	MaxWeaponAmmo = 70;
	BaseWeaponRange = 4000.0f;
	CurrentWeaponAmmo = MaxWeaponAmmo;
}

void AMySMGWeaponActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearTimer(SMGFIreTimer);
}

void AMySMGWeaponActor::WeaponAttack()
{
	GetWorldTimerManager().SetTimer(SMGFIreTimer, this, &AMySMGWeaponActor::HandleSMGFire, SMGFireRate, true);

}

void AMySMGWeaponActor::WeaponReload()
{
	Super::WeaponReload();
}

void AMySMGWeaponActor::WeaponAnim()
{
	
}

void AMySMGWeaponActor::WeaponStopAttack()
{
	//SMG�߻�����ó��
	GetWorldTimerManager().ClearTimer(SMGFIreTimer);
}

void AMySMGWeaponActor::HandleSMGFire()
{
	UE_LOG(LogTemp, Warning, TEXT("AMMO : %d"), CurrentWeaponAmmo);
	GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Blue, FString::Printf(TEXT("AMMO : %d"), CurrentWeaponAmmo));

	if (CurrentWeaponAmmo > 0)
	{
		CurrentWeaponAmmo = CurrentWeaponAmmo - 1;
	}
	//ź����� 0�̸� �߻� ���� �� ������
	if (CurrentWeaponAmmo <= 0)
	{
		WeaponStopAttack();
		WeaponReload();
		return;
	}

	// ī�޶� ������Ʈ ����
	UCameraComponent* activeCamComp = CameraComp;

	//LineTrace�� ������ġ(Ȱ��ȭ�� ī�޶� ��ġ)
	FVector startPos = activeCamComp->GetComponentLocation();
	//LineTrace�� ������ġ(Ȱ��ȭ�� ī�޶� ���߾ӿ��� �������� SMGRange��ŭ)
	FVector endPos = startPos + (activeCamComp->GetForwardVector() * BaseWeaponRange);
	//LineTrace�� �浹������ ���� ����
	FHitResult hitInfo;
	//�浹�ɼ� ��������
	FCollisionQueryParams params;

	//�� �ڽ��� �浹 ����
	params.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	//Channel ���͸� �̿��� LineTrace �浹 ����
	//(�浹����, ������ġ, ������ġ, ����ä��, �浹�ɼ�)
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

	// ���̾ư��� �ý��� ����
	UNiagaraComponent* TrailNSC = nullptr;
	if (WeaponTrail)
	{
		FVector TraceVector = hitInfo.ImpactPoint - startPos;
		FVector SpawnPos = startPos + (TraceVector * 0.5f); // 50% �������� ����
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

		//������ ���Ϳ� ������ ����(�÷��̾� ATK �ݿ�)
		UGameplayStatics::ApplyPointDamage(hitInfo.GetActor(), BaseWeaponDamage * AttackPower, activeCamComp->GetForwardVector(), HitResult, UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetController(), this, nullptr);

		//�ǰݹ�ü ���������� ����
		auto hitComp = hitInfo.GetComponent();
		//1. ���� ���� ������Ʈ�� ������ ����Ǿ��ִٸ�
		if (hitComp && hitComp->IsSimulatingPhysics())
		{
			//2. �������� ���� ������ ������ �����ְ�
			FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 5000;
			//3. �� �������� ����������.
			hitComp->AddForce(force);
		}

		// bHit�� ������ 0.3�� ������ ���̾ư��� �ý��� ��Ȱ��ȭ
		if (TrailNSC)
		{
			GetWorld()->GetTimerManager().SetTimer(WeaponEffectTimerHandle, [TrailNSC]()
				{
					if (TrailNSC)
					{
						TrailNSC->Deactivate();
					}
				}, 0.3f, false);
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
