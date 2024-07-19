// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWeapon/MySMGWeaponActor.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h" //추가
#include "NiagaraComponent.h" //추가
#include "NiagaraFunctionLibrary.h" //추가

AMySMGWeaponActor::AMySMGWeaponActor()
{
	//충돌체크기설정
	WeaponBoxComp->SetBoxExtent(FVector(15.0f));
	//3-2. 스켈레탈메시 데이터로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempSMGMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MarketplaceBlockout/Modern/Weapons/Assets/SMGs/01/SKM_Modern_Weapons_SMG_01.SKM_Modern_Weapons_SMG_01'"));
	//3-3. 데이터로드가 성공했다면
	if (TempSMGMesh.Succeeded())
	{
		//3-4. 스켈레탈메시 데이터 할당
		GunMesh->SetSkeletalMesh(TempSMGMesh.Object);
	}

}

void AMySMGWeaponActor::BeginPlay()
{
	//SMG기본설정
	SMGFireRate = 0.1f;  //1초에 10발
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
	//SMG발사중지처리
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
	//탄약수가 0이면 발사 중지 및 재장전
	if (CurrentWeaponAmmo <= 0)
	{
		WeaponStopAttack();
		WeaponReload();
		return;
	}

	// 카메라 컴포넌트 선택
	UCameraComponent* activeCamComp = CameraComp;

	//LineTrace의 시작위치(활성화된 카메라 위치)
	FVector startPos = activeCamComp->GetComponentLocation();
	//LineTrace의 종료위치(활성화된 카메라 정중앙에서 전방으로 SMGRange만큼)
	FVector endPos = startPos + (activeCamComp->GetForwardVector() * BaseWeaponRange);
	//LineTrace의 충돌정보를 담을 변수
	FHitResult hitInfo;
	//충돌옵션 설정변수
	FCollisionQueryParams params;

	//나 자신은 충돌 무시
	params.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	//Channel 필터를 이용한 LineTrace 충돌 검출
	//(충돌정보, 시작위치, 종료위치, 검출채널, 충돌옵션)
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);

	// 나이아가라 시스템 스폰
	UNiagaraComponent* TrailNSC = nullptr;
	if (WeaponTrail)
	{
		FVector TraceVector = hitInfo.ImpactPoint - startPos;
		FVector SpawnPos = startPos + (TraceVector * 0.5f); // 50% 지점에서 스폰
		FRotator TrailRotation = TraceVector.Rotation();
		TrailNSC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), WeaponTrail, SpawnPos, TrailRotation);
	}

	//LineTrace가 부딪쳤을때(bHit가 true일 때)
	if (bHit)
	{
		//충돌처리 -> 총알 파편효과 재생
		//총알파편효과 트랜스폼 변수 선언
		FTransform bulletTrans;
		//부딪친 위치 할당
		bulletTrans.SetLocation(hitInfo.ImpactPoint);
		//총알 파편 효과 인스턴스 생성
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

		//적중한 액터에 데미지 적용(플레이어 ATK 반영)
		UGameplayStatics::ApplyPointDamage(hitInfo.GetActor(), BaseWeaponDamage * AttackPower, activeCamComp->GetForwardVector(), HitResult, UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetController(), this, nullptr);

		//피격물체 날려보내기 구현
		auto hitComp = hitInfo.GetComponent();
		//1. 만약 맞은 컴포넌트에 물리가 적용되어있다면
		if (hitComp && hitComp->IsSimulatingPhysics())
		{
			//2. 날려버릴 힘의 정도와 방향을 정해주고
			FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 5000;
			//3. 그 방향으로 날려보낸다.
			hitComp->AddForce(force);
		}

		// bHit가 됐을때 0.3초 지연후 나이아가라 시스템 비활성화
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

	// 총구 화염 파티클 생성
	if (MuzzleFlash)
	{
		FTransform MuzzleTransform = GunMesh->GetSocketTransform(TEXT("FirePosition"));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
	}

}
