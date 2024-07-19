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

	// 플레이어를 찾는다.
	ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	// 플레이어를 향해 방향을 변경한다.
	FVector direction = (player->GetActorLocation() - GetActorLocation());
	direction.Normalize();

	// 총알을 발사한다.
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

		// 데미지를 입는다.
		ATPSPlayer* player = Cast<ATPSPlayer>(OtherActor);
		player->TakeDamage(10.0f, FDamageEvent(), GetInstigatorController(), this);

		// 총알을 제거한다.
		Die();
	}
}

