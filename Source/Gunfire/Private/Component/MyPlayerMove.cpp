// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/MyPlayerMove.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"


UMyPlayerMove::UMyPlayerMove()
{
	//Tick�Լ� ȣ��ǵ��� ó��
	PrimaryComponentTick.bCanEverTick = true;

	//�޸��� ���� �ʱ�ȭ
	bisRunning = false;
}

void UMyPlayerMove::BeginPlay()
{
	Super::BeginPlay();

	//�ʱ� �ӵ��� �ٱ�� ����
	moveComp->MaxWalkSpeed = runSpeed;
}

void UMyPlayerMove::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MyMove();
}

void UMyPlayerMove::SetupInputBinding(UEnhancedInputComponent* PlayerInput)
{
	//ī�޶� ȸ��ó�� �̺�Ʈó���Լ� ���ε�
	PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &UMyPlayerMove::Turn);	
	PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &UMyPlayerMove::LookUp);
	//�����¿��Է� �̺�Ʈó���Լ� ���ε�
	PlayerInput->BindAction(ia_PlayerMove, ETriggerEvent::Triggered, this, &UMyPlayerMove::InputMove);
	//�޸����Է� �̺�Ʈó���Լ� ���ε�
	PlayerInput->BindAction(ia_Run, ETriggerEvent::Triggered, this, &UMyPlayerMove::InputRun);
	//�����Է� �̺�Ʈó���Լ� ���ε�
	PlayerInput->BindAction(ia_PlayerJump, ETriggerEvent::Triggered, this, &UMyPlayerMove::InputJump);
}


//���콺 ��Ʈ�� �̺�Ʈó���Լ� ����
void UMyPlayerMove::Turn(const struct FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerYawInput(value);
}
void UMyPlayerMove::LookUp(const struct FInputActionValue& inputValue)
{
	float value = inputValue.Get<float>();
	me->AddControllerPitchInput(value);
}


//�����¿��̵� �Է��̺�Ʈ ó���Լ�
void UMyPlayerMove::InputMove(const struct FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	//�����Է��̺�Ʈó��
	direction.X = value.X;
	//�¿��Է��̺�Ʈó��
	direction.Y = value.Y;
}

//�̵��Լ� Move(����ȭ)
void UMyPlayerMove::MyMove()
{
	//������ǥ�� �����ǥ�� ����
	direction = FTransform(me->GetControlRotation()).TransformVector(direction);
	//�𸮾󿡼� �����ϴ� �⺻ �����Ʈ ������Ʈ ���
	me->AddMovementInput(direction);
	direction = FVector::ZeroVector; //�ʱ�ȭ
}

//�޸��� �̺�Ʈó�� �Լ� ����
void UMyPlayerMove::InputRun()
{
	//�޸��� ���¸� ��� ������� ����
	bisRunning = !bisRunning;

	//�޸�����¿� ���� �ӵ�����
	if (bisRunning)
	{
		me->GetCharacterMovement()->MaxWalkSpeed = runSpeed;
	}
	else
	{
		me->GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	}
}

//Jump �̺�Ʈó���Լ� ����
void UMyPlayerMove::InputJump(const struct FInputActionValue& inputValue)
{
	me->Jump();
}