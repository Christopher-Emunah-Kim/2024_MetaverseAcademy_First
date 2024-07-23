// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageAmt.generated.h"

UCLASS()
class GUNFIRE_API ADamageAmt : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageAmt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// 블루프린트에서 구현하는 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "Damage")
	void SetDamageText(float Damage, bool isCritical);
};
