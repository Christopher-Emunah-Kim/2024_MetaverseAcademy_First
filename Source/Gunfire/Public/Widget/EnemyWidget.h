// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API UEnemyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindEnemyStat(class UEnemyStatComponent* EnemyStatComponent);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void UpdateHPWidget();

public:
	void SetDecreaseRate(float NewRate) { DecreaseRate = NewRate; }

private:
	TWeakObjectPtr<class UEnemyStatComponent> CurrentEnemyStat;

	float CurrentPercent = 1.0f;
	float TargetPercent = 1.0f;
	float DecreaseRate = 1.0f;

	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HPProgressBarBack;
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HPProgressBar;
};
