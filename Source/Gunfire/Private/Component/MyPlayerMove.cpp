// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/MyPlayerMove.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"


UMyPlayerMove::UMyPlayerMove()
{
	//Tick함수 호출되도록 처리
	PrimaryComponentTick.bCanEverTick = true;

	//달리기 상태 초기화
	bisRunning = false;
}

void UMyPlayerMove::BeginPlay()
{
	Super::BeginPlay();

	//초기 속도를 뛰기로 설정
	moveComp->MaxWalkSpeed = runSpeed;
}

void UMyPlayerMove::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MyMove();
}

void UMyPlayerMove::SetupInputBinding(UEnhancedInputComponent* PlayerInput)
{
	//카메라 회전처리 이벤트처리함수 바인딩
	PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &UMyPlayerMove::Turn);	
	PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &UMyPlayerMove::LookUp);
	//상하좌우입력 이벤트처리함수 바인딩
	PlayerInput->BindAction(ia_PlayerMove, ETriggerEvent::Triggered, this, &UMyPlayerMove::InputMove);
	//달리기입력 이벤트처리함수 바인딩
	PlayerInput->BindAction(ia_Run, ETriggerEvent::Triggered, this, &UMyPlayerMove::InputRun);
	//점프입력 이벤트처리함수 바인딩
	PlayerInput->BindAction(ia_PlayerJump, ETriggerEvent::Triggered, this, &UMyPlayerMove::InputJump);
}


//마우스 컨트롤 이벤트처리함수 구현
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


//상하좌우이동 입력이벤트 처리함수
void UMyPlayerMove::InputMove(const struct FInputActionValue& inputValue)
{
	FVector2D value = inputValue.Get<FVector2D>();
	//상하입력이벤트처리
	direction.X = value.X;
	//좌우입력이벤트처리
	direction.Y = value.Y;
}

//이동함수 Move(최적화)
void UMyPlayerMove::MyMove()
{
	//절대좌표를 상대좌표로 변경
	direction = FTransform(me->GetControlRotation()).TransformVector(direction);
	//언리얼에서 제공하는 기본 무브먼트 컴포넌트 사용
	me->AddMovementInput(direction);
	direction = FVector::ZeroVector; //초기화
}

//달리기 이벤트처리 함수 구현
void UMyPlayerMove::InputRun()
{
	//달리기 상태를 토글 방식으로 변경
	bisRunning = !bisRunning;

	//달리기상태에 따라 속도변경
	if (bisRunning)
	{
		me->GetCharacterMovement()->MaxWalkSpeed = runSpeed;
	}
	else
	{
		me->GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	}
}

//Jump 이벤트처리함수 구현
void UMyPlayerMove::InputJump(const struct FInputActionValue& inputValue)
{
	me->Jump();
}