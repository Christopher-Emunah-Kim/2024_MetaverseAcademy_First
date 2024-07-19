// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
//��������~~~

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//1. �浹ü ���
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	//2. �浹 �������� ����
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));
	//3 �浹ü ũ�� ����
	collisionComp->SetSphereRadius(13);
	//4. ��Ʈ�� ���
	RootComponent = collisionComp;

	//5. �ܰ� ������Ʈ ���
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));
	//6. �θ�������Ʈ ����
	bodyMeshComp->SetupAttachment(collisionComp);
	//7. �޽� �浹ü ��Ȱ��ȭ
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//8. �޽� �ܰ� ũ�� ����
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));

	//9. �߻�ü ������Ʈ
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	//10. movement ������Ʈ�� ���Ž�ų ������Ʈ ����
	movementComp->SetUpdatedComponent(collisionComp);
	//11. �߻�ü �ʱ�ӵ� ����
	movementComp->InitialSpeed = 5000;
	//12. �߻�ü �ִ�ӵ� ����
	movementComp->MaxSpeed = 5000;
	//13. �߻�ü �ݵ�����
	movementComp->bShouldBounce = true;
	//14. �߻�ü �ݵ���
	movementComp->Bounciness = 0.3f;

	//�⺻������ �ʱ�ȭ
	Damage = 10.0f;

}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	//Timer�� ���� Die �Լ� ȣ��(�Ѿ� ����)
	FTimerHandle deathTimer;
	GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &ABullet::Die, 2.0f, false);
	
}

//�Ѿ� �ν��Ͻ����� �Լ� ����
void ABullet::Die()
{
	Destroy();
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


//������ Ư���Ӽ��� ����� ��� ȣ��Ǵ� �̺�Ʈ �Լ�
void ABullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	//speed���� �����Ǿ����� üũ
	if (PropertyChangedEvent.GetPropertyName() == TEXT("speed"))
	{
		//������Ÿ�� �����Ʈ ������Ʈ�� ������ speed�� ����
		movementComp->InitialSpeed = speed;
		movementComp->MaxSpeed = speed;
	}
}
