// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/EnemyWidget.h"
#include "Component/EnemyStatComponent.h"
#include "Components/ProgressBar.h"

void UEnemyWidget::BindEnemyStat(UEnemyStatComponent* EnemyStatComponent)
{
	CurrentEnemyStat = EnemyStatComponent;

	EnemyStatComponent->OnHPChanged.AddUObject(this, &UEnemyWidget::UpdateHPWidget);
	UpdateHPWidget();

	if (HPProgressBarBack != nullptr)
	{
		HPProgressBarBack->SetPercent(1.0f);
	}
}

void UEnemyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	/*HPProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PB_HPBar")));*/
	// UpdateHPWidget();
}

void UEnemyWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (CurrentPercent > TargetPercent)
	{
		CurrentPercent -= DecreaseRate * InDeltaTime;
		if (CurrentPercent < TargetPercent)
		{
			CurrentPercent = TargetPercent;
		}

		if (HPProgressBarBack != nullptr)
		{
			HPProgressBarBack->SetPercent(CurrentPercent);
		}
	}
}

void UEnemyWidget::UpdateHPWidget()
{
	if (CurrentEnemyStat.IsValid())
	{
		if (HPProgressBar != nullptr)
		{
			HPProgressBar->SetPercent(CurrentEnemyStat->GetHPRatio());
			TargetPercent = CurrentEnemyStat->GetHPRatio();
		}
	}
}
