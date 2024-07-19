// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/MyPlayerBaseComponent.h"

// Sets default values for this component's properties
UMyPlayerBaseComponent::UMyPlayerBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//tick에 작동하지않도록 false처리
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UMyPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//컴포넌트 소유중인 액터의 인스턴스 가져오기
	me = Cast<ATPSPlayer>(GetOwner());
	//me에서 무빙관련 인스턴스 얻어오기
	moveComp = me->GetCharacterMovement();
	
}


