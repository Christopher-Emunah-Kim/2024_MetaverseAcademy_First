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

	//1. �浹ü ���
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	//2. �浹ü ũ�� ����
	collisionComp->InitSphereRadius(15.0f);
	//3. ��Ʈ�� ���
	RootComponent = collisionComp;

	//4. �ܰ� ������Ʈ ���
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	//5. �θ�������Ʈ ����
	bodyMeshComp->SetupAttachment(collisionComp);
	//6. �޽� �浹ü ��Ȱ��ȭ
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//7. �޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<UStaticMesh> GrenadeMesh(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Props/SM_Rock.SM_Rock'"));
	if (GrenadeMesh.Succeeded())
	{
		bodyMeshComp->SetStaticMesh(GrenadeMesh.Object);
	}
	//8. �޽� �ܰ� ũ�� ����
	bodyMeshComp->SetRelativeScale3D(FVector(0.5f));

	//9. �߻�ü ������Ʈ ���
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	//10. �ڵ�Ȱ��ȭ
	movementComp->bAutoActivate = true;
	//11. movement������Ʈ�� ���Ž�ų ������Ʈ ����
	movementComp->SetUpdatedComponent(collisionComp);
	//12. �߻�ü �ʱ�ӵ� ����
	movementComp->InitialSpeed = 500.0f;
	//13. �߻�ü �ִ�ӵ� ����
	movementComp->MaxSpeed = 900.0f;
	//14. ȸ�� ������Ʈ ����(�߻�üȸ���� Velocity����� ��ġ)
	movementComp->bRotationFollowsVelocity = true;

	//�⺻������ �ʱ�ȭ
	Damage = 80.0f;

}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	//�߻� �� ���������ð� ����
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AGrenade::OnTimerExpire, 4.0f, false);
}

// Called every frame
void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//���� ó���Լ� ����
void AGrenade::OnTimerExpire()
{
	Explode();
	Destroy();
}


//�����Լ� ����
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