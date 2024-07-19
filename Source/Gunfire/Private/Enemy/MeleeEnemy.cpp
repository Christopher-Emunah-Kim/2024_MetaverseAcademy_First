// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/MeleeEnemy.h"
#include "Engine/DamageEvents.h"

#include "Component/EnemyStatComponent.h"

AMeleeEnemy::AMeleeEnemy()
: AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("Sword"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Game/Enemys/MeleeEnemy/Weapons/Sword/SK_Straight_Sword.SK_Straight_Sword"));
	if (tempMesh.Succeeded())
	{
		WeaponMesh->SetSkeletalMesh(tempMesh.Object);
	}

	ShieldMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMeshComp"));
	ShieldMeshComp->SetupAttachment(GetMesh(), TEXT("Shield"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempStaticMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempStaticMesh.Succeeded())
	{
		ShieldMeshComp->SetStaticMesh(tempStaticMesh.Object);
	}

	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyHPStat != nullptr)
		EnemyHPStat->InitHP(300);
	UE_LOG(LogTemp, Warning, TEXT("MeleeEnemy BeginPlay"));
}

void AMeleeEnemy::OnAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("OnAttack"));
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 150,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(150),
		Params);

#if ENABLE_DRAW_DEBUG

	/*FVector TraceVec = GetActorForwardVector() * 150;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = 150 * 0.5f + 150;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 0.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		150,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);*/

#endif

	if (bResult)
	{
		AActor* actor = HitResult.GetActor();
		if (actor != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor Name : %s"), *actor->GetName());

			FDamageEvent DamageEvent;
			actor->TakeDamage(10, DamageEvent, GetController(), this);
		}
	}
}
