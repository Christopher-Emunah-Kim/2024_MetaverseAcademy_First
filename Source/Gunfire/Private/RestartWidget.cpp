// Fill out your copyright notice in the Description page of Project Settings.


#include "RestartWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void URestartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BottonContinue->OnClicked.AddDynamic(this, &URestartWidget::OnBottonContinue);
}

void URestartWidget::OnBottonContinue()
{
	FString mapName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	UGameplayStatics::OpenLevel(GetWorld(), FName(*mapName));
}
