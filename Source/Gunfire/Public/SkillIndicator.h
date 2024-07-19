// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillIndicator.generated.h"

UCLASS()
class GUNFIRE_API ASkillIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkillIndicator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* DynamicMaterialMesh;


	float RunningTime;

	// 시전 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CastingTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DestroyTime = 3.0f;
};
