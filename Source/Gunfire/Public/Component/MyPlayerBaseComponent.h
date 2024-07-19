// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSPlayer.h" //추가
#include <GameFramework/CharacterMovementComponent.h> //추가
#include "MyPlayerBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNFIRE_API UMyPlayerBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMyPlayerBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	//사용자 입력 맵핑 처리 가상함수선언
	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput){};

	//컴포넌트 소유액터
	UPROPERTY()
	ATPSPlayer* me;

	//이동관련 객체 생성
	UPROPERTY()
	UCharacterMovementComponent* moveComp;
		
};
