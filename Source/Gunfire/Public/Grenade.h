// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Grenade.generated.h"

UCLASS()
class GUNFIRE_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Explode();
	UProjectileMovementComponent* GetProjectileMovementComponent() const { return movementComp; }

public:
	//�߻�ü �̵��� ����� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* movementComp;
	//�浹ü ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = Collision)
	class USphereComponent* collisionComp;
	//�ܰ� ������Ʈ
	UPROPERTY(VisibleAnywhere, Category = BodyMesh)
	class UStaticMeshComponent* bodyMeshComp;

	//������ ó�� ������Ʈ
	/*UPROPERTY(EditDefaultsOnly, Category = Damage)
	float ExplosionDamage;*/
	//������ ���� ������Ʈ
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	float ExplosionRadius;

	//������ ���� �߰�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	float Damage;


	//���� ó�� �Լ� ����
	UFUNCTION()
	void OnTimerExpire();

	//���� �ð� ó�� ���� ����
	FTimerHandle TimerHandle;
};
