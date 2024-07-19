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
	//발사체 이동을 담당할 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* movementComp;
	//충돌체 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Collision)
	class USphereComponent* collisionComp;
	//외관 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = BodyMesh)
	class UStaticMeshComponent* bodyMeshComp;

	//데미지 처리 컴포넌트
	/*UPROPERTY(EditDefaultsOnly, Category = Damage)
	float ExplosionDamage;*/
	//데미지 범위 컴포넌트
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	float ExplosionRadius;

	//데미지 변수 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	float Damage;


	//폭발 처리 함수 선언
	UFUNCTION()
	void OnTimerExpire();

	//폭발 시간 처리 변수 선언
	FTimerHandle TimerHandle;
};
