// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/GolemBossEnemy.h"
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
#include "AnimInstance/EnemyAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AGolemBossEnemy::AGolemBossEnemy()
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

	LeftPunch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftPunch"));
	LeftPunch->SetupAttachment(GetMesh(), TEXT("LeftPunch"));

	RightPunch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightPunch"));
	RightPunch->SetupAttachment(GetMesh(), TEXT("RightPunch"));

	// 씬컴포넌트 초기화
	StoneSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StoneSpawnPoint"));
	StoneSpawnPoint->SetupAttachment(GetMesh(), TEXT("StoneSpawnPoint"));

	// 크리티컬 포인트 박스 메쉬 만들기SocketCopyPasteBuffer

	EyeBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("EyeBoxComponent"));
	EyeBoxComponent->SetupAttachment(GetMesh(), TEXT("EyeBox"));
	EyeBoxComponent->SetBoxExtent(FVector(500.0f, 100.0f, 100.0f));

	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	EyeBoxComponent->AttachToComponent(GetMesh(), Rules, TEXT("Bip001_HeadSocket"));
	EyeBoxComponent->ComponentTags.Add(FName("Head"));
	// 심장 박스 메쉬 만들기

	HeartBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HeartBoxComponent"));
	HeartBoxComponent->SetupAttachment(GetMesh(), TEXT("HeartBox"));
	HeartBoxComponent->SetBoxExtent(FVector(500.0f, 500.0f, 500.0f));
	HeartBoxComponent->AttachToComponent(GetMesh(), Rules, TEXT("Bip001_Spine1Socket"));
	//테그 추가
	HeartBoxComponent->ComponentTags.Add(FName("Head"));

	// 오른 손 스메쉬 생성
	SmashEffectPointRight = CreateDefaultSubobject<USceneComponent>(TEXT("SmashEffectPointRight"));
	SmashEffectPointRight->SetupAttachment(RootComponent);
	// 위치 2500, -1000
	SmashEffectPointRight->SetRelativeLocation(FVector(2500.0f, -1000.0f, 0.0f));

	// 왼 손 스메쉬 생성
	SmashEffectPointLeft = CreateDefaultSubobject<USceneComponent>(TEXT("SmashEffectPointLeft"));
	SmashEffectPointLeft->SetupAttachment(RootComponent);
	// 위치 2500, 1000
	SmashEffectPointLeft->SetRelativeLocation(FVector(2500.0f, 1000.0f, 0.0f));

	PatternList.Add(0);
	PatternList.Add(1);
	PatternList.Add(2);
}

void AGolemBossEnemy::BeginPlay()
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

	if (EnemyWidget != nullptr)
	{
		EnemyWidget->SetDecreaseRate(0.1f);
		EnemyWidget->BindEnemyStat(EnemyHPStat);
	}
}

void AGolemBossEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 클리어 타이머
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
}

void AGolemBossEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (MyAnim)
	{
		MyAnim->OnChasingStone.AddLambda([this]() -> void {
			DoChasingStone();
			});

		MyAnim->OnPunchReady.AddLambda([this]() -> void {
			DoPunchReady();
			});

		MyAnim->OnSmash.AddLambda([this]() -> void {
			DoSmash();
			});

		MyAnim->OnSummon.AddLambda([this]() -> void {
			DoSummon();
			});

		MyAnim->OnLaser.AddLambda([this]() -> void {
			DoLaser();
			});

		MyAnim->OnPunchFireL.AddLambda([this]() -> void {
			DoPunchFireL();
			});

		MyAnim->OnPunchFireR.AddLambda([this]() -> void {
			DoPunchFireR();
			});
	}
}

void AGolemBossEnemy::DoAttack()
{
	// 애니메이션 실행
	OnAttack();
}

void AGolemBossEnemy::OnAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("OnAttack"));
	SelectPattern();
}

void AGolemBossEnemy::SelectPattern()
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

void AGolemBossEnemy::EndPattern()
{
 	UE_LOG(LogTemp, Warning, TEXT("EndPattern"));
	OnAttackEnd.Broadcast();
}

void AGolemBossEnemy::PatternRockPillar()
{
	PatternRotting();
	bSummon = true;
}

void AGolemBossEnemy::PatternChasingStone()
{
	bChasingStone = true;
}

void AGolemBossEnemy::PatternLaserAttack()
{
	bLaser = true;
}

void AGolemBossEnemy::PatternSmash()
{
	UE_LOG(LogTemp, Warning, TEXT("PatternSmash"));
	bSmash = true;
}

void AGolemBossEnemy::PatternRocketPunch()
{
	UE_LOG(LogTemp, Warning, TEXT("PatternRocketPunch"));
	bRocket = true;
}

void AGolemBossEnemy::DoSmash()
{
	bSmash = false;
	bPillar = false;
	for (TActorIterator<AEnemySkill_RockPillar> It(GetWorld()); It; ++It)
	{
		AEnemySkill_RockPillar* rock = *It;
		if (rock)
		{
			rock->OnExplosion();
		}
	}

	// 스메쉬 위치에 스메쉬 생성
	GetWorld()->SpawnActor<AActor>(SmashEffectFactory, SmashEffectPointRight->GetComponentLocation(), SmashEffectPointRight->GetComponentRotation());
	GetWorld()->SpawnActor<AActor>(SmashEffectFactory, SmashEffectPointLeft->GetComponentLocation(), SmashEffectPointLeft->GetComponentRotation());
	
	EndPattern();
}

void AGolemBossEnemy::DoSummon()
{
	UE_LOG(LogTemp, Warning, TEXT("PatternRockPillar"));

	bSummon = false;
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

	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &AGolemBossEnemy::EndPattern, 5.0f, false);
}

void AGolemBossEnemy::DoLaser()
{
	// 심장 위치에 레이저 스폰
	FVector Location = Heart->GetComponentLocation();
	FRotator Rotation = Heart->GetComponentRotation();

	// 레이저 스폰
	GetWorld()->SpawnActor<AEnemySkill_Laser>(LaserAttackClass, Location, Rotation);
}

void AGolemBossEnemy::DoChasingStone()
{
	bChasingStone = false;
	FVector Center = StoneSpawnPoint->GetComponentLocation();

	UGameplayStatics::PlaySound2D(GetWorld(), ChasingStoneSound);

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

	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &AGolemBossEnemy::EndPattern, 5.0f, false);
}

void AGolemBossEnemy::DoPunchReady()
{
	//bRocket = false;
	//DoRocketPunch();
	DoRocketAiming();
}

void AGolemBossEnemy::DoPunchFireL()
{
	bRocket = false;
	DoRocketPunchL();
}

void AGolemBossEnemy::DoPunchFireR()
{
	DoRocketPunchR();
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &AGolemBossEnemy::EndPattern, 3.0f, false);
}

void AGolemBossEnemy::PatternRotting()
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

void AGolemBossEnemy::EndLaserAttack()
{
	bLaser = false;
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &AGolemBossEnemy::EndPattern, 2.0f, false);
}
