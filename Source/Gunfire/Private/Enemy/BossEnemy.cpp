// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossEnemy.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Engine/TargetPoint.h"
#include "EnemySkill/EnemySkill_RockPillar.h"
#include "EnemySkill/EnemySkill_ChasingStone.h"
#include "EnemySkill/EnemySkill_RocketPunch.h"
#include "Components/StaticMeshComponent.h"
#include "EnemySkill/EnemySkill_Laser.h"
#include "Blueprint/UserWidget.h"
#include "Widget/EnemyWidget.h"
#include "Component/EnemyStatComponent.h"

ABossEnemy::ABossEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<AActor> StonePillarBP(TEXT("/Game/BluePrints/Enemy/Pattern/BP_EnemyRockPillar.BP_EnemyRockPillar_C"));
	if (StonePillarBP.Succeeded())
	{
		RockPillarClass = StonePillarBP.Class;
	}
	// 초기화
	static ConstructorHelpers::FClassFinder<AActor> ChasingStoneBP(TEXT("/Game/BluePrints/Enemy/Pattern/BP_EnemyChasingStone.BP_EnemyChasingStone_C"));
	if (ChasingStoneBP.Succeeded())
	{
		ChasingStoneClass = ChasingStoneBP.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> LaserAttackBP(TEXT("/Game/BluePrints/Enemy/Pattern/BP_EnemyLaser.BP_EnemyLaser_C"));
	if (LaserAttackBP.Succeeded())
	{
		LaserAttackClass = LaserAttackBP.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget>
		BossHPWidgetBP(TEXT("/Game/BluePrints/Enemy/BP_BossEnemyWidget.BP_BossEnemyWidget_C"));
	if (BossHPWidgetBP.Succeeded())
	{
		BossHPWidgetClass = BossHPWidgetBP.Class;
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Widget class found!"));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Widget class not found!"));
		}
	}

	// 씬컴포넌트 초기화
	Heart = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Heart"));
	Heart->SetupAttachment(GetMesh(), TEXT("Heart"));
	// 좌표 변경(X=14.000000,Y=0.000000,Z=20.000000)
	Heart->SetRelativeLocation(FVector(14.0f, 0.0f, 20.0f));

	// 씬컴포넌트 초기화
	StoneSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StoneSpawnPoint"));
	StoneSpawnPoint->SetupAttachment(GetMesh(), TEXT("StoneSpawnPoint"));

	// 씬컴포넌트 좌표 변경
	StoneSpawnPoint->SetRelativeLocation(FVector(70.0f, 0.0f, 50.0f));

	LeftHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftHand"));
	LeftHand->SetupAttachment(GetMesh(), TEXT("LeftHand"));

	RightHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightHand"));
	RightHand->SetupAttachment(GetMesh(), TEXT("RightHand"));

	bPillar = false;

	PatternList.Add(0);
	PatternList.Add(1);
	PatternList.Add(2);
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (EnemyHPStat)
		EnemyHPStat->InitHP(5000);
	bPillar = false;
	bLaser = false;
	CurrentPattern = 0;

	BossHPWidget = CreateWidget(GetWorld(), BossHPWidgetClass);
	if (BossHPWidget != nullptr)
		BossHPWidget->AddToViewport();

	UEnemyWidget* EnemyWidget = Cast<UEnemyWidget>(BossHPWidget);

	if (EnemyWidget !=nullptr)
	{
		EnemyWidget->SetDecreaseRate(0.1f);
		EnemyWidget->BindEnemyStat(EnemyHPStat);
	}
}

void ABossEnemy::DoAttack()
{
	// 애니메이션 실행
	OnAttack();
}

void ABossEnemy::OnAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("OnAttack"));
	SelectPattern();
}

void ABossEnemy::SelectPattern()
{
	if (!bPillar)
	{
		PatternRockPillar();
	}
	else
	{
		if (CurrentPattern >= PatternList.Num())
		{
			PatternSmash();
			CurrentPattern = 0;
			return;
		}

		switch (PatternList[CurrentPattern])
		{
		case 0:
			PatternRocketPunch();
			break;
		case 1:
			PatternChasingStone();
			break;
		case 2:
			PatternLaserAttack();
			break;
		default:
			break;
		}

		CurrentPattern++;
	}

}

void ABossEnemy::EndPattern()
{
	UE_LOG(LogTemp, Warning, TEXT("EndPattern"));
	OnAttackEnd.Broadcast();
}

void ABossEnemy::PatternRockPillar()
{
	PatternRotting();

	UE_LOG(LogTemp, Warning, TEXT("PatternRockPillar"));

	bPillar = true;
	int32 RandomInt = FMath::RandRange(0, 3);
	int i = 0;
	for (TActorIterator<ATargetPoint> It(GetWorld()); It; ++It)
	{
		i++;
		if (i % 4 == RandomInt)
		{
			ATargetPoint* TargetPoint = *It;
			if (TargetPoint)
			{
				FVector Location = TargetPoint->GetActorLocation();
				FRotator Rotation = TargetPoint->GetActorRotation();

				// 타겟 포인트 위치에 기둥을 생성합니다. 
				// 돌기둥을 생성합니다.
				AEnemySkill_RockPillar* StonePillar = GetWorld()->SpawnActor<AEnemySkill_RockPillar>(RockPillarClass, Location, Rotation);
			}
		}
	}

	FTimerHandle CooldownTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ABossEnemy::EndPattern, 5.0f, false);
}

void ABossEnemy::PatternChasingStone()
{
	FVector Center = StoneSpawnPoint->GetComponentLocation();

	UE_LOG(LogTemp, Warning, TEXT("PatternChasingStone"));
	for (int32 i = 0; i < 7; i++)
	{
		float Angle = FMath::RandRange(0.0f, 360.0f);
		float Distance = FMath::RandRange(0.0f, 700.0f);
		float Rad = FMath::DegreesToRadians(Angle);

		float X = Center.X + Distance * FMath::Cos(Rad);
		float Y = Center.Y + Distance * FMath::Sin(Rad);

		FVector SpawnLocation = FVector(X, Y, Center.Z);
		GetWorld()->SpawnActor<AEnemySkill_ChasingStone>(ChasingStoneClass, SpawnLocation, FRotator::ZeroRotator);
	}


	FTimerHandle CooldownTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ABossEnemy::EndPattern, 5.0f, false);
}

void ABossEnemy::PatternLaserAttack()
{
	bLaser = true;

	// 심장 위치에 레이저 스폰
	FVector Location = Heart->GetComponentLocation();
	FRotator Rotation = Heart->GetComponentRotation();

	// 레이저 스폰
	GetWorld()->SpawnActor<AEnemySkill_Laser>(LaserAttackClass, Location, Rotation);
	
	/*FTimerHandle CooldownTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ABossEnemy::EndPattern, 2.0f, false);*/
}

void ABossEnemy::PatternSmash()
{
	UE_LOG(LogTemp, Warning, TEXT("PatternSmash"));
	bPillar = false;
	for (TActorIterator<AEnemySkill_RockPillar> It(GetWorld()); It; ++It)
	{
		AEnemySkill_RockPillar* rock = *It;
		if (rock)
		{
			rock->OnExplosion();
		}
	}

	FTimerHandle CooldownTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ABossEnemy::EndPattern, 3.0f, false);
}

void ABossEnemy::PatternRocketPunch()
{
	UE_LOG(LogTemp, Warning, TEXT("PatternRocketPunch"));

	DoRocketPunch();

	FTimerHandle CooldownTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ABossEnemy::EndPattern, 6.0f, false);
}

void ABossEnemy::PatternRotting()
{
	CurrentPattern = 0;
	// 랜덤 셔플 마지막 3번은 변경하지 않는다.
	for (int i = 0; i < PatternList.Num(); i++)
	{
		int32 RandomInt = FMath::RandRange(0, PatternList.Num() - 1);
		int32 Temp = PatternList[i];
		PatternList[i] = PatternList[RandomInt];
		PatternList[RandomInt] = Temp;
	}
}

void ABossEnemy::EndLaserAttack()
{
	FTimerHandle CooldownTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &ABossEnemy::EndPattern, 2.0f, false);
}
