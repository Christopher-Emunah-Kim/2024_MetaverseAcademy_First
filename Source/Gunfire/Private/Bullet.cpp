// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
//잊지말기~~~

// Sets default values
ABullet::ABullet()
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
	movementComp->InitialSpeed = 5000;
	//12. 발사체 최대속도 설정
	movementComp->MaxSpeed = 5000;
	//13. 발사체 반동여부
	movementComp->bShouldBounce = true;
	//14. 발사체 반동값
	movementComp->Bounciness = 0.3f;

	//기본데미지 초기화
	Damage = 10.0f;

}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	//Timer를 통한 Die 함수 호출(총알 삭제)
	FTimerHandle deathTimer;
	GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &ABullet::Die, 2.0f, false);
	
}

//총알 인스턴스삭제 함수 구현
void ABullet::Die()
{
	Destroy();
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


//액터의 특정속성이 변경될 경우 호출되는 이벤트 함수
void ABullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	//speed값이 수정되었는지 체크
	if (PropertyChangedEvent.GetPropertyName() == TEXT("speed"))
	{
		//프로젝타일 무브먼트 컴포넌트에 수정된 speed값 적용
		movementComp->InitialSpeed = speed;
		movementComp->MaxSpeed = speed;
	}
}
