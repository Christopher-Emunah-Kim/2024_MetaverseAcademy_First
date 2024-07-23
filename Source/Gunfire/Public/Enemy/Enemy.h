// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class GUNFIRE_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

	// virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void DoAttack();
	virtual void OnAttack();
	
	UFUNCTION()
	void AttackEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	virtual void AimEnemy(bool bAiming);


	FOnAttackEndDelegate OnAttackEnd;


	UPROPERTY()
	class UEnemyAnimInstance* MyAnim;

	UPROPERTY(EditAnywhere, Category = Status)
	class UEnemyStatComponent* EnemyHPStat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	class UWidgetComponent* HPBarWidget;

	//총알공장 속성 추가
	UPROPERTY(EditDefaultsOnly, Category = BulletFactory)
	TSubclassOf<class AProjectile> bulletFactory;

	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* FireMontage;

	//현재 무기 액터 인스턴스
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMyWeaponActor> DropWeaponFactory;

	UMaterialInstanceDynamic* DynamicMaterialInstance;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ADamageAmt> DamageAmtFactory;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ADamageAmt> CriticalDamageAmtFactory;
};
