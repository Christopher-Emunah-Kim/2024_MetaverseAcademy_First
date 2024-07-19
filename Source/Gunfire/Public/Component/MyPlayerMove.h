// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/MyPlayerBaseComponent.h"
#include "MyPlayerMove.generated.h"

/**
 * 
 */
UCLASS()
class GUNFIRE_API UMyPlayerMove : public UMyPlayerBaseComponent
{
	GENERATED_BODY()

public:
	UMyPlayerMove();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_LookUp;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Turn;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_PlayerMove;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_Run;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ia_PlayerJump;
	
	virtual void SetupInputBinding(class UEnhancedInputComponent* PlayerInput) override;

	//�¿� ȸ�� �Է�ó��(����)
	void Turn(const struct FInputActionValue& inputValue);
	//���� ȸ�� �Է�ó��(����)
	void LookUp(const struct FInputActionValue& inputValue);
	//�����¿��̵� �Է��̺�Ʈ ó���Լ�
	void InputMove(const struct FInputActionValue& inputValue);
	//���� �Է��̺�Ʈ ó���Լ�
	void InputJump(const struct FInputActionValue& inputValue);

	//�̵�����
	FVector direction;

	//�ȱ�ӵ�
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float walkSpeed = 250;
	//�޸���ӵ�
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float runSpeed = 500;
	//�޸������ ���庯�� �߰�
	bool bisRunning;

	//�÷��̾� �̵�ó��
	void MyMove();

	//�޸��� �̺�Ʈ ó�� �Լ�
	void InputRun();

};
