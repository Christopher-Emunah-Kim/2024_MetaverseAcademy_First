// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//1. 충돌체 등록
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	//2. 충돌체 크기 설정
	collisionComp->InitSphereRadius(15.0f);
	//3. 루트로 등록
	RootComponent = collisionComp;

	//4. 외관 컴포넌트 등록
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	//5. 부모컴포넌트 설정
	bodyMeshComp->SetupAttachment(collisionComp);
	//6. 메시 충돌체 비활성화
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//7. 메시 데이터 로드
	ConstructorHelpers::FObjectFinder<UStaticMesh> GrenadeMesh(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Rock.SM_Rock'"));
	if (GrenadeMesh.Succeeded())
	{
		bodyMeshComp->SetStaticMesh(GrenadeMesh.Object);
	}
	//8. 메시 외관 크기 설정
	bodyMeshComp->SetRelativeScale3D(FVector(0.5f));

	//9. 발사체 컴포넌트 등록
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	//10. 자동활성화
	movementComp->bAutoActivate = true;
	//11. movement컴포넌트가 갱신시킬 컴포넌트 지정
	movementComp->SetUpdatedComponent(collisionComp);
	//12. 발사체 초기속도 설정
	movementComp->InitialSpeed = 500.0f;
	//13. 발사체 최대속도 설정
	movementComp->MaxSpeed = 900.0f;
	//14. 회전 업데이트 설정(발사체회전이 Velocity방향과 일치)
	movementComp->bRotationFollowsVelocity = true;

	//기본데미지 초기화
	Damage = 80.0f;

}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	//발사 후 폭발지연시간 설정
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AGrenade::OnTimerExpire, 4.0f, false);
}

// Called every frame
void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//폭발 처리함수 구현
void AGrenade::OnTimerExpire()
{
	Explode();
	Destroy();
}


//폭발함수 구현
void AGrenade::Explode()
{
	UGameplayStatics::ApplyRadialDamage(
		this,
		Damage,
		GetActorLocation(),
		ExplosionRadius,
		UDamageType::StaticClass(),
		TArray<AActor*>(),
		this,
		GetInstigatorController(),
		true
	);
}