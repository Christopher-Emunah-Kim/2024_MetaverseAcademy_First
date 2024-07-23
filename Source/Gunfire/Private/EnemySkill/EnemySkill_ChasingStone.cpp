// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySkill/EnemySkill_ChasingStone.h"

#include "Kismet/GameplayStatics.h"
#include "Bullet.h"
// 폰 인클루드
#include "GameFramework/Pawn.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/DamageType.h"
#include "Components/StaticMeshComponent.h"
#include "EnemySkill/EnemySkill_RockPillar.h"
#include "Widget/DamageAmt.h"

// Sets default values
AEnemySkill_ChasingStone::AEnemySkill_ChasingStone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Stone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stone"));
	RootComponent = Stone;
	Stone->OnComponentBeginOverlap.AddDynamic(this, &AEnemySkill_ChasingStone::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AEnemySkill_ChasingStone::BeginPlay()
{
	Super::BeginPlay();
	Hp = 80.0f;
}

// Called every frame
void AEnemySkill_ChasingStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemySkill_ChasingStone::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Hp -= FinalDamage;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = nullptr;
	auto* DamageAmt = GetWorld()->SpawnActor<ADamageAmt>(DamageAmtFactory, GetActorLocation(), FRotator::ZeroRotator);
	DamageAmt->SetDamageText(FinalDamage, true);

	if (Hp <= 0)
	{
		Destroy();
		// 스폰 액터
		GetWorld()->SpawnActor<AActor>(ExplosionEffect, GetActorLocation(), GetActorRotation());
	}

	return FinalDamage;
}

void AEnemySkill_ChasingStone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* Player = Cast<APawn>(OtherActor);
	if (Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Overlapped"));
		Player->TakeDamage(10.0f, FDamageEvent(), nullptr, this);

		// 스폰 액터
		GetWorld()->SpawnActor<AActor>(ExplosionEffect, GetActorLocation(), GetActorRotation());
		Destroy();
	}

	if (OtherActor->IsA<AEnemySkill_RockPillar>())
	{
		// 스폰 액터
		GetWorld()->SpawnActor<AActor>(ExplosionEffect, GetActorLocation(), GetActorRotation());
		Destroy();
	}
}
