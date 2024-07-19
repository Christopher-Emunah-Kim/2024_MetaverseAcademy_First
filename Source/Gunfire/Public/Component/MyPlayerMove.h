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

	//좌우 회전 입력처리(수정)
	void Turn(const struct FInputActionValue& inputValue);
	//상하 회전 입력처리(수정)
	void LookUp(const struct FInputActionValue& inputValue);
	//상하좌우이동 입력이벤트 처리함수
	void InputMove(const struct FInputActionValue& inputValue);
	//점프 입력이벤트 처리함수
	void InputJump(const struct FInputActionValue& inputValue);

	//이동방향
	FVector direction;

	//걷기속도
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float walkSpeed = 250;
	//달리기속도
	UPROPERTY(EditAnywhere, Category=PlayerSetting)
	float runSpeed = 500;
	//달리기상태 저장변수 추가
	bool bisRunning;

	//플레이어 이동처리
	void MyMove();

	//달리기 이벤트 처리 함수
	void InputRun();

};
