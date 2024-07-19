// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSPlayer.h" //�߰�
#include <GameFramework/CharacterMovementComponent.h> //�߰�
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
	//����� �Է� ���� ó�� �����Լ�����
	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput){};

	//������Ʈ ��������
	UPROPERTY()
	ATPSPlayer* me;

	//�̵����� ��ü ����
	UPROPERTY()
	UCharacterMovementComponent* moveComp;
		
};
