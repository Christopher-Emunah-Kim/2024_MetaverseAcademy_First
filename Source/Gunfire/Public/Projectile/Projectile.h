// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class GUNFIRE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

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

	void Die();
	
	UPROPERTY(EditAnywhere, Category = Settings)
	float speed = 5000;

	//������ Ư�� �Ӽ��� �����ϸ� ȣ��Ǵ� �̺�Ʈ �Լ�
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	// Projectile ������
	enum class EOwnerType
	{
		Player,
		Enemy,
		Ally
	};

	// Projectile ������ Ÿ��
	EOwnerType OwnerType;
};
