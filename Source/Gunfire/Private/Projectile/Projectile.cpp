// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/Projectile.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
AProjectile::AProjectile()
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
	movementComp->InitialSpeed = 500;
	//12. �߻�ü �ִ�ӵ� ����
	movementComp->MaxSpeed = 500;
	//13. �߻�ü �ݵ�����
	movementComp->bShouldBounce = true;
	//14. �߻�ü �ݵ���
	movementComp->Bounciness = 0.3f;
	// �߷� ����
	movementComp->ProjectileGravityScale = 0.0f;
}

void AProjectile::Die()
{
	Destroy();
}

void AProjectile::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//�Ѿ� �ӵ� ����
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AProjectile, speed))
	{
		movementComp->InitialSpeed = speed;
		movementComp->MaxSpeed = speed;
	}
}

