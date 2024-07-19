// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
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
	virtual void PostInitializeComponents() override;

	// �ִϸ��̼� ����
	virtual void DoAttack() override;
	// ���� ������ �Ͼ
	virtual void OnAttack() override;

	// ���� ����
	void SelectPattern();
	// ���� ����
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

	// ������ ����.
	void PatternRotting();
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

	// scene component
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* LeftPunch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* RightPunch;

	// ������ �߻� ��ġ
	UPROPERTY(EditAnywhere)
	USceneComponent* StoneSpawnPoint;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Heart;
	
	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<class UUserWidget> BossHPWidgetClass;

	UPROPERTY()
	class UUserWidget* BossHPWidget;
	

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
};
