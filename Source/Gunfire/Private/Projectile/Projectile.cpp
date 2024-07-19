// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/Projectile.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//1. 충돌체 등록
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	//2. 충돌 프로파일 설정
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	//3 충돌체 크기 설정
	collisionComp->SetSphereRadius(13);
	//4. 루트로 등록
	RootComponent = collisionComp;

	//5. 외관 컴포넌트 등록
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	//6. 부모컴포넌트 지정
	bodyMeshComp->SetupAttachment(collisionComp);
	//7. 메시 충돌체 비활성화
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//8. 메시 외관 크기 설정
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));

	//9. 발사체 컴포넌트
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	//10. movement 컴포넌트가 갱신시킬 컴포넌트 지정
	movementComp->SetUpdatedComponent(collisionComp);
	//11. 발사체 초기속도 설정
	movementComp->InitialSpeed = 500;
	//12. 발사체 최대속도 설정
	movementComp->MaxSpeed = 500;
	//13. 발사체 반동여부
	movementComp->bShouldBounce = true;
	//14. 발사체 반동값
	movementComp->Bounciness = 0.3f;
	// 중력 제거
	movementComp->ProjectileGravityScale = 0.0f;
}

void AProjectile::Die()
{
	Destroy();
}

void AProjectile::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//총알 속도 변경
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AProjectile, speed))
	{
		movementComp->InitialSpeed = speed;
		movementComp->MaxSpeed = speed;
	}
}

