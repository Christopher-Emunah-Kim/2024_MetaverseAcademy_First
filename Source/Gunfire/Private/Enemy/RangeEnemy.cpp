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
}

void ARangeEnemy::OnAttack()
{
	//�� ���̷�Ż�޽� ������Ʈ�� �ѱ���ġ ������ �����´�.
	FTransform firePosition = WeaponMesh->GetSocketTransform(TEXT("FirePosition"));
	//�ѱ���ġ�� �Ѿ˰����� ��ġ�ϰ�, �ű⿡�� �Ѿ��� �����Ѵ�.
	GetWorld()->SpawnActor<AProjectile>(bulletFactory, firePosition);
}
