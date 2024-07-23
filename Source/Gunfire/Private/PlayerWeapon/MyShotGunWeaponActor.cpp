// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWeapon/MyShotGunWeaponActor.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h" //추가
#include "NiagaraComponent.h" //추가
#include "NiagaraFunctionLibrary.h" //추가

AMyShotGunWeaponActor::AMyShotGunWeaponActor()
{
	//충돌체크기설정
	WeaponBoxComp->SetBoxExtent(FVector(15.0f));
	//4-2. 스켈레탈메시 데이터로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempShotGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MarketplaceBlockout/Modern/Weapons/Assets/Shotguns/02/SKM_Modern_Weapons_Shotgun_02.SKM_Modern_Weapons_Shotgun_02'"));
	//4-3. 데이터로드가 성공했다면
	if (TempShotGunMesh.Succeeded())
	{
		//4-4. 스켈레탈메시 데이터 할당
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

		//총알 발사소리를 플레이한다.
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponShotSound);

	}
	else
	{
		//총알 빈탄창소리를 플레이한다.
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponNoBulletSound);

		//그냥 자동 재장전을 원한다면..
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
	// 카메라 컴포넌트 선택
	UCameraComponent* activeCamComp = CameraComp;

	FVector startPos = activeCamComp->GetComponentLocation();
	FVector forwardVector = activeCamComp->GetForwardVector();

	//원뿔형으로 뻗어나가도록 각도 조정
	float horizontalAngleStep = 4.0f; //수평각도 간격
	float verticalAngleStep = 4.0f;   //수직각도 간격
	int32 numTraces = 8; //라인트레이스 갯수

	//10개의 라인트레이스선이 뻗어나가도록 생성(샷건모드)
	for (int32 i = 0; i < numTraces; i++)
	{
		//무작위각도를 생성하여 원뿔형태로 퍼지도록 함
		float randomYaw = FMath::RandRange(-horizontalAngleStep, horizontalAngleStep);
		float randomPitch = FMath::RandRange(-verticalAngleStep, verticalAngleStep);

		//각도를 회전벡터로 변환
		FRotator randomRotator = FRotator(randomPitch, randomYaw, 0.0f);
		FVector rotatedVector = randomRotator.RotateVector(forwardVector);
		FVector endPos = startPos + (rotatedVector * BaseWeaponRange);

		//충돌정보 저장
		FHitResult hitInfo;
		FCollisionQueryParams params;
		//나 자신은 충돌 무시
		params.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

		//라인트레이스실행 및 설정에 따른 충돌 및 데미지 적용
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

		// 나이아가라 시스템 스폰
		UNiagaraComponent* TrailNSC = nullptr;
		if (WeaponTrail)
		{
			FVector TraceVector = hitInfo.ImpactPoint - startPos;
			FVector SpawnPos = startPos + (TraceVector * 0.7f); // 70% 지점에서 스폰
			FRotator TrailRotation = TraceVector.Rotation();
			TrailNSC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WeaponTrail, SpawnPos, TrailRotation);
		}

		if (bHit)
		{
			// 충돌 처리 -> 총알 파편 효과 재생
			FTransform bulletTrans;
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			// 적중한 액터에 데미지 적용
			UGameplayStatics::ApplyPointDamage(hitInfo.GetActor(), BaseWeaponDamage * AttackPower, rotatedVector, hitInfo, UGameplayStatics::GetPlayerPawn(GetWorld(),0)->GetController(), this, nullptr);


			// 피격 물체 날려보내기 구현
			auto hitComp = hitInfo.GetComponent();
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 3000;
				hitComp->AddForce(force);
			}

			// bHit가 됐을때 0.2초 지연후 나이아가라 시스템 비활성화
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
		//발사상태 설정
		bIsFiring = true;

		// 총구 화염 파티클 생성
		if (MuzzleFlash)
		{
			FTransform MuzzleTransform = GunMesh->GetSocketTransform(TEXT("FirePosition"));
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
		};
	}
}
