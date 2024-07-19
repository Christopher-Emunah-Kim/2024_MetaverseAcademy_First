// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile/EnemyBulletProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Character.h>
#include "TPSPlayer.h"
#include "Engine/DamageEvents.h"

// Called when the game starts or when spawned
void AEnemyBulletProjectile::BeginPlay()
{
	Super::BeginPlay();

	// �÷��̾ ã�´�.
	ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	// �÷��̾ ���� ������ �����Ѵ�.
	FVector direction = (player->GetActorLocation() - GetActorLocation());
	direction.Normalize();

	// �Ѿ��� �߻��Ѵ�.
	movementComp->Velocity = direction * speed;
}

// Called every frame
void AEnemyBulletProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyBulletProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor && OtherActor->IsA(ATPSPlayer::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit"));

		// �������� �Դ´�.
		ATPSPlayer* player = Cast<ATPSPlayer>(OtherActor);
		player->TakeDamage(10.0f, FDamageEvent(), GetInstigatorController(), this);

		// �Ѿ��� �����Ѵ�.
		Die();
	}
}

