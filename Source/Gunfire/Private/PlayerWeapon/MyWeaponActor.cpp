// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWeapon/MyWeaponActor.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "NiagaraSystem.h" //추가
#include "NiagaraComponent.h" //추가
#include "NiagaraFunctionLibrary.h" //추가
#include "Kismet/GameplayStatics.h" //추가

// Sets default values
AMyWeaponActor::AMyWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Weapon 충돌체설정
	WeaponBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBoxComp"));
	WeaponBoxComp->SetCollisionProfileName(TEXT("Weapon"));
	SetRootComponent(WeaponBoxComp);
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(WeaponBoxComp);
	WeaponSpawnEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WeaponSpawnEffect"));
	WeaponSpawnEffect->SetupAttachment(WeaponBoxComp);
}

// Called when the game starts or when spawned
void AMyWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMyWeaponActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(WeaponEffectTimerHandle);

}

// Called every frame
void AMyWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyWeaponActor::WeaponAttack()
{
	
}

void AMyWeaponActor::WeaponReload()
{
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, [this]()
		{
			CurrentWeaponAmmo = MaxWeaponAmmo;

		}, 1.0f, false);

	//총알 Reload소리를 플레이한다.
	UGameplayStatics::PlaySound2D(GetWorld(), WeaponReloadSound);
}

void AMyWeaponActor::WeaponAnim()
{
}

void AMyWeaponActor::WeaponStopAttack()
{
}

void AMyWeaponActor::SetPlayerBuff(float value)
{
	AttackPower = value;
}

void AMyWeaponActor::SetCamera(UCameraComponent* currentCamComp)
{
	CameraComp = currentCamComp;
}

