// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API ABossEnemy : public AEnemy
{
	GENERATED_BODY()
	
public:
	ABossEnemy();
	
	virtual void BeginPlay() override;

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
	
	// ������ ����.
	void PatternRotting();
	UFUNCTION(BlueprintImplementableEvent, Category = "CppCall")
	void DoRocketPunch();


	UFUNCTION(BlueprintCallable, Category = "CppCall")
	void EndLaserAttack();

	int32 CurrentPattern;

	bool bPillar;
	bool bLaser;

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
	USkeletalMeshComponent* LeftHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* RightHand;

	// ������ �߻� ��ġ
	UPROPERTY(EditAnywhere)
	USceneComponent* StoneSpawnPoint;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Heart;
	
	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<class UUserWidget> BossHPWidgetClass;

	UPROPERTY()
	class UUserWidget* BossHPWidget;

	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundBase* ChasingStoneSound;
};
