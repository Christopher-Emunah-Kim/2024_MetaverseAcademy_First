// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillIndicator.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ASkillIndicator::ASkillIndicator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DynamicMaterialMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DynamicMaterialMesh")); 
	DynamicMaterialMesh->CreateDynamicMaterialInstance(0);
}

// Called when the game starts or when spawned
void ASkillIndicator::BeginPlay()
{
	Super::BeginPlay();
	RunningTime = 0;

	// 바닥을 찾고 살짝 위쪽으로 이동시킨다.
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, 1000);
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
	SetActorLocation(HitResult.Location + FVector(0, 0, 10));
}

// Called every frame
void ASkillIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
	
	if (RunningTime < CastingTime)
	{
		float duration = RunningTime / CastingTime;
		DynamicMaterialMesh->SetScalarParameterValueOnMaterials(TEXT("_Duration"), duration);
	}

	if (RunningTime > DestroyTime)
	{
		Destroy();
	}
}

