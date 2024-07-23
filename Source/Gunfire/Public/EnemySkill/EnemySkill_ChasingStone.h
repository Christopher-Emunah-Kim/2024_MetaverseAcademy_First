// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySkill_ChasingStone.generated.h"

UCLASS()
class GUNFIRE_API AEnemySkill_ChasingStone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySkill_ChasingStone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
    void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Stone;
	UPROPERTY(EditAnywhere)
	float Hp;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ExplosionEffect;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ADamageAmt> DamageAmtFactory;
};
