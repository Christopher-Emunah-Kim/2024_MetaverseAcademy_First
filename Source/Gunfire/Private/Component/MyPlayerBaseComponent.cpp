// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/MyPlayerBaseComponent.h"

// Sets default values for this component's properties
UMyPlayerBaseComponent::UMyPlayerBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//tick�� �۵������ʵ��� falseó��
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UMyPlayerBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//������Ʈ �������� ������ �ν��Ͻ� ��������
	me = Cast<ATPSPlayer>(GetOwner());
	//me���� �������� �ν��Ͻ� ������
	moveComp = me->GetCharacterMovement();
	
}


