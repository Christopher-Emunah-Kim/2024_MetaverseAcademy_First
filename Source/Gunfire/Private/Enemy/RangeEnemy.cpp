// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/RangeEnemy.h"
#include "Projectile/Projectile.h"
#include "Component/EnemyStatComponent.h"

ARangeEnemy::ARangeEnemy()
	: AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));

	if (tempMesh.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(tempMesh.Object);
	}

	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void ARangeEnemy::BeginPlay()
{
	Super::BeginPlay();

	EnemyHPStat->SetHP(200);

	UMaterialInterface* Material = GetMesh()->GetMaterial(0);
	if (Material)
	{
		// Dynamic Material Instance 생성
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(Material, this);

		// Static Mesh에 Dynamic Material Instance 적용
		GetMesh()->SetMaterial(0, DynamicMaterialInstance);

		// Emissive Color 파라미터 값을 변경합니다.
		FLinearColor NewEmissiveColor = FLinearColor(0, 0, 0, 0);
		DynamicMaterialInstance->SetVectorParameterValue(FName("_Color"), NewEmissiveColor);// Emissive Color 파라미터 값을 변경합니다.
		DynamicMaterialInstance->SetScalarParameterValue(FName("_Expo"), 0);
	}
}

void ARangeEnemy::OnAttack()
{
	//총 스켈레탈메시 컴포넌트의 총구위치 정보를 가져온다.
	FTransform firePosition = WeaponMesh->GetSocketTransform(TEXT("FirePosition"));
	//총구위치에 총알공장이 위치하고, 거기에서 총알을 생성한다.
	GetWorld()->SpawnActor<AProjectile>(bulletFactory, firePosition);
}

void ARangeEnemy::AimEnemy(bool bAiming)
{
	if (DynamicMaterialInstance)
	{
		if (bAiming)
		{
			// Emissive Color 파라미터 값을 변경합니다.
			FLinearColor NewEmissiveColor = FLinearColor(5, 0, 0, 0);
			DynamicMaterialInstance->SetVectorParameterValue(FName("_Color"), NewEmissiveColor);// Emissive Color 파라미터 값을 변경합니다.
			DynamicMaterialInstance->SetScalarParameterValue(FName("_Expo"), 5);
		}
		else
		{
			// Emissive Color 파라미터 값을 변경합니다.
			FLinearColor NewEmissiveColor = FLinearColor(0, 0, 0, 0);
			DynamicMaterialInstance->SetVectorParameterValue(FName("_Color"), NewEmissiveColor);// Emissive Color 파라미터 값을 변경합니다.
			DynamicMaterialInstance->SetScalarParameterValue(FName("_Expo"), 0);
		}
	}
}
