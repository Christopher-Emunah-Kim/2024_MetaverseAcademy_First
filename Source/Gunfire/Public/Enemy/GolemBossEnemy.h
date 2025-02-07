// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "ControlRig.h"
#include "ControlRigComponent.h"
#include "GolemBossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API AGolemBossEnemy : public AEnemy
{
	GENERATED_BODY()
	public:
	AGolemBossEnemy();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;

	// 애니메이션 실행
	virtual void DoAttack() override;
	// 실제 공격이 일어남
	virtual void OnAttack() override;

	// 패턴 선택
	void SelectPattern();
	// 패턴 종료
	void EndPattern();

	void PatternRockPillar();
	void PatternChasingStone();
	void PatternLaserAttack();
	void PatternSmash();
	void PatternRocketPunch();

	void DoSmash();
	void DoSummon();
	void DoLaser();
	void DoChasingStone();
	void DoPunchReady();
	void DoPunchFireL();
	void DoPunchFireR();

	// 패턴을 셔플.
	void PatternRotting();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "CppCall")
	void DoRocketAiming();

	UFUNCTION(BlueprintImplementableEvent, Category = "CppCall")
	void DoRocketPunchL();

	UFUNCTION(BlueprintImplementableEvent, Category = "CppCall")
	void DoRocketPunchR();

	UFUNCTION(BlueprintCallable, Category = "CppCall")
	void EndLaserAttack();

	int32 CurrentPattern;

	UPROPERTY()
	TArray<int32> PatternList;

	UPROPERTY()
	TSubclassOf<AActor> RockPillarClass;

	UPROPERTY()
	TSubclassOf<AActor> ChasingStoneClass;

	UPROPERTY()
	TSubclassOf<AActor> LaserAttackClass;

	// 돌맹이 발사 위치
	UPROPERTY(EditAnywhere)
	USceneComponent* StoneSpawnPoint;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Heart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* LeftPunch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* RightPunch;

	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<class UUserWidget> BossHPWidgetClass;

	UPROPERTY()
	class UUserWidget* BossHPWidget;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* EyeBoxComponent;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* HeartBoxComponent;
	
	FTimerHandle CooldownTimerHandle;

	FVector OriginalPunchLocationL;
	FVector OriginalPunchLocationR;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPillar;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSmash;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bLaser;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRocket;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bFire;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bChasingStone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bSummon;
	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundBase* ChasingStoneSound;

	// 이펙트 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AActor> SmashEffectFactory;

	// 오른손 스매시 이펙트 생성 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* SmashEffectPointRight;

	// 왼손 스매시 이펙트 생성 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* SmashEffectPointLeft;

};
