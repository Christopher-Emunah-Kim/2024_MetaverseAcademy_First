// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include "EnhancedInputSubsystems.h" //추가
#include "EnhancedInputComponent.h" //추가
#include "InputActionValue.h"  //추가
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Blueprint/UserWidget.h>
#include "Bullet.h"
#include "Grenade.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Components/ProgressBar.h> 
#include <Components/TextBlock.h> 
#include <Component/MyPlayerMove.h> //추가
#include "PlayerWeapon/MyWeaponActor.h"
#include "Components/BoxComponent.h"
#include <PlayerWeapon/MyEmptyWeaponActor.h>
#include "Components/CapsuleComponent.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region Component Setting

	playerMove = CreateDefaultSubobject<UMyPlayerMove>(TEXT("MyPlayerMove"));

#pragma endregion

#pragma region TPS Character Data Load

	//TPS캐릭터 스켈레탈메시 데이터를 불러오기
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Animation_Starter/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));

	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);

		//2. Mesh 컴포넌트 위치와 회전값을 설정
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

#pragma endregion

#pragma region Camera Settings

	//기본 카메라 설정
	//SpringArm 컴포넌트 붙이기
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	//팩토리함수로 인스턴스 생성하여 SpringArmComp변수에 저장
	springArmComp->SetupAttachment(RootComponent);
	//Root컴포넌트 자식으로 설정
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	//부모 컴포넌트 기준으로 상대좌표 설정
	springArmComp->TargetArmLength = 400;
	//타깃과의 지정거리 설정
	springArmComp->bUsePawnControlRotation = true;
	//회전처리 설정값 등록

	//TPS 카메라를 붙이고 싶다
	//Camera 컴포넌트 붙이기
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	//팩토리함수로 인스턴스 생성하여 TpsCamComp변수에 저장
	tpsCamComp->SetupAttachment(springArmComp);
	//springCamComp의 자식으로 등록
	tpsCamComp->bUsePawnControlRotation = false;
	//회전처리 설정값 등록

	//FPS 카메라를 붙이고 싶다. 
	//Camera 컴포넌트 붙이기
	fpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FpsCamComp"));
	//팩토리함수로 인스턴스 생성하여 fpsCamComp변수에 저장
	fpsCamComp->SetupAttachment(GetMesh(), TEXT("headSocket"));
	//카메라를 플레이어 메시에 부착
	fpsCamComp->SetRelativeLocation(FVector(0, 20, 0));
	fpsCamComp->SetRelativeRotation(FRotator(0, 90, -90));
	//부모컴포넌트 기준으로 위치/회전 설정
	fpsCamComp->bUsePawnControlRotation = true;
	//회전처리 설정값 등록
	fpsCamComp->SetActive(false);

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	//클래스 회전처리 설정값 등록
	//실제 카메라가 돌아갈때 캐릭터가 함께 돌아갈지 아닐지를 같이 설정

	//1인칭/3인칭시점 전환 기본값 설정
	bIsFirstPerson = true;

#pragma endregion

#pragma region FPS Character Data Load
//fpscamcomp에 붙이는거라 위치 옮기면 안됨.

	//FPS캐릭터 메시 컴포넌트 초기화
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSMesh"));
	FPSMesh->SetupAttachment(fpsCamComp);
	FPSMesh->SetRelativeLocation(FVector(50.0f, 0.0f, -170.0f));
	FPSMesh->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	FPSMesh->SetOnlyOwnerSee(true); // FPS 메시를 플레이어만 볼 수 있도록 설정
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	//FPS메시 스켈레탈 메시 데이터로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> FPSMeshObject(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/FirstPersonArms/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'"));
	if (FPSMeshObject.Succeeded())
	{
		FPSMesh->SetSkeletalMesh(FPSMeshObject.Object);
	}

#pragma endregion

#pragma region Weapon Data Load

	//1. 유탄총 스켈레탈메시 컴포넌트 등록
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	//1-1. 부모 컴포넌트를 Mesh컴포넌트로 설정
	gunMeshComp->SetupAttachment(FPSMesh, TEXT("GripPoint"));
	//1-2. 유탄총기 스켈레탈메시 데이터 로드
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	//1-3. 데이터로드가 성공했다면
	if (TempGunMesh.Succeeded())
	{
		//1-4. 스켈레탈 메시 데이터 할당
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
	}

	////2. 스나이퍼건 컴포넌트 등록
	//sniperGunComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SniperGunComp"));
	////2-1. 부모 컴포넌트를 Mesh 컴포넌트로 설정
	//sniperGunComp->SetupAttachment(FPSMesh, TEXT("GripPoint"));
	////2-2. 스켈레탈메시 데이터 로드
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> TempSniperMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MarketplaceBlockout/Modern/Weapons/Assets/Snipers/02/SKM_Modern_Weapons_Sniper_02.SKM_Modern_Weapons_Sniper_02'"));
	////2-3. 데이터로드가 성공했다면
	//if (TempSniperMesh.Succeeded())
	//{
	//	//2-4. 로드한 스태틱메시 데이터 할당
	//	sniperGunComp->SetSkeletalMesh(TempSniperMesh.Object);
	//}

	//스나이퍼모드에서 타임라인 삽입
	//2-7. 타임라인 컴포넌트 초기화
	ZoomTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("ZoomTimeLine"));
	//2-8. 무기 위치 및 회전 초기화
	OriginalGunTransform = FTransform::Identity;
	//2-9 원하는 목표 위치 및 회전으로 설정
	TargetGunTransform = FTransform(FRotator(7.0f, 5.0f, 3.0f), FVector(15.0f, -70.0f, 23.0f)); 
     

	////3. SMG 컴포넌트 등록
	//SMGMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SMGComp"));
	////3-1. 부모컴포넌트를 설정
	//SMGMesh->SetupAttachment(FPSMesh, TEXT("GripPoint"));
	////3-2. 스켈레탈메시 데이터로드
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> TempSMGMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MarketplaceBlockout/Modern/Weapons/Assets/SMGs/01/SKM_Modern_Weapons_SMG_01.SKM_Modern_Weapons_SMG_01'"));
	////3-3. 데이터로드가 성공했다면
	//if (TempSMGMesh.Succeeded())
	//{
	//	//3-4. 스켈레탈메시 데이터 할당
	//	SMGMesh->SetSkeletalMesh(TempSMGMesh.Object);
	//}

	////SMG기본설정
	//SMGFireRate = 0.1f;  //1초에 10발
	////SMGDamage = 10.0f;
	//SMGRange = 4000.0f;

	////4. ShotGun 컴포넌트 등록
	//ShotGunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShotGunComp"));
	////4-1. 부모컴포넌트를 설정
	//ShotGunMesh->SetupAttachment(FPSMesh, TEXT("GripPoint"));
	////4-2. 스켈레탈메시 데이터로드
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> TempShotGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MarketplaceBlockout/Modern/Weapons/Assets/Shotguns/02/SKM_Modern_Weapons_Shotgun_02.SKM_Modern_Weapons_Shotgun_02'"));
	////4-3. 데이터로드가 성공했다면
	//if (TempShotGunMesh.Succeeded())
	//{
	//	//4-4. 스켈레탈메시 데이터 할당
	//	ShotGunMesh->SetSkeletalMesh(TempShotGunMesh.Object);
	//}
	//
	////ShotGun 기본 설정
	//ShotGunRange = 2000.0f;

#pragma endregion
	
#pragma region Movement Settings

	//2단 점프 구현
	JumpMaxCount = 2;
	//초기 상태를 대시 가능으로 설정
	bCanLaunch = true;
	//기본마찰력상태 변수 초기화
	OriginalGroundFriction = GetCharacterMovement()->GroundFriction;
	//달리기 상태 초기화
	//bisRunning = false;

	// DashEffectScene 컴포넌트 생성
	DashEffectScene = CreateDefaultSubobject<USceneComponent>(TEXT("DashEffectScene"));
	DashEffectScene->SetupAttachment(RootComponent); // RootComponent에 부착
	DashEffectScene->SetRelativeLocation(FVector(200, 0, 90)); // 원하는 위치 설정

	//대쉬 이펙트 나이아가라 컴포넌트 생성 
	PlayerDashEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PlayerDashEffect"));
	PlayerDashEffect->SetupAttachment(DashEffectScene);
	PlayerDashEffect->bAutoActivate = false; // 기본적으로 비활성화
	
#pragma endregion

#pragma region Player/Weapon Base Setting

	//플레이어의 기본 공격력 설정
	AttackPower = 1.0f;
	//플레이어의 기본 HP설정
	PlayerMAXHP = 300.0f;
	PlayerHP = PlayerMAXHP;

	//플레이어의 ATK스탯에 영향을 받는 무기별 기본데미지 설정
	BaseGrenadeGunDamage = 50.0f;
	/*BaseSniperGunDamage = 100.0f;
	BaseSMGDamage = 15.0f;
	BaseShotGunDamage = 15.0f;*/
	BaseGrenadeDamage = 150.0f;

	//수류탄 기본설정
	ThrowingForce = 800.0f;

#pragma endregion

}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 게임 재시작시 게임 일시정지 풀기
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	// 입력 모드 설정 초기화
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;
	}

	//Enhanced Input Context 등록
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}


	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::OnMyOverlapBegin);

	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ATPSPlayer::OnMyOverlapEnd);

	////초기 속도를 걷기로 설정
	//GetCharacterMovement()->MaxWalkSpeed = runSpeed;

	//1-1. 스나이퍼 UI 위젯 인스턴스 생성
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	//1-2. 타임라인 곡선이 설정되어있는지 확인
	if (ZoomCurve)
	{
		//1-3. 타임라인에 곡선 추가 및 업데이트 함수 바인딩
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("ZoomTimelineProgress"));
		ZoomTimeLine->AddInterpFloat(ZoomCurve, ProgressFunction);
	}
	//1-4. 무기의 원래위치와 회전 저장
	OriginalGunTransform = gunMeshComp->GetRelativeTransform();

	//일반조준 UI 생성
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	//PlayerHUD UI 생성
	_playerHUDUI = CreateWidget(GetWorld(), playerHUDFactory);
	//데미지 UI 생성
	_takedamageUI = CreateWidget(GetWorld(), TDamageUIFactory);
	//RestartUI 생성 
	_restartUI = CreateWidget(GetWorld(), RestartUIFactory);
	//LoadingUI 생성 및 화면에 추가
	_loadingUI = CreateWidget(GetWorld(), LoadingUIFactory);
	
	_takedamageUI->AddToViewport(1);
	_crosshairUI->AddToViewport(2);
	_playerHUDUI->AddToViewport(3);
	_loadingUI->AddToViewport(4);
	_restartUI->AddToViewport(10);

	_loadingUI->SetVisibility(ESlateVisibility::Visible);
	_takedamageUI->SetVisibility(ESlateVisibility::Hidden);
	_crosshairUI->SetVisibility(ESlateVisibility::Hidden);
	_playerHUDUI->SetVisibility(ESlateVisibility::Hidden);
	_restartUI->SetVisibility(ESlateVisibility::Hidden);

	GetWorldTimerManager().SetTimer(LoadingHandle, this, &ATPSPlayer::ShowMainUI, 3.0f, false);

	//초기 플레이어 시점 설정
	tpsCamComp->SetActive(!bIsFirstPerson);
	fpsCamComp->SetActive(bIsFirstPerson);

	//초기 시점 설정에 따라 메시 가시성 설정
	GetMesh()->SetVisibility(!bIsFirstPerson, true);
	FPSMesh->SetVisibility(bIsFirstPerson, true);

	////무기슬롯 초기화(기본으로 SMG를 1번)
	//WeaponSlots.Empty(3);
	//WeaponSlots.Add(EWeaponType::SMG);
	//WeaponSlots.Add(EWeaponType::SniperGun);
	//WeaponSlots.Add(EWeaponType::ShotGun);

	//기본으로 SMG을 사용하도록 설정
	//ChangeWeapon(0);

	//// 초기 탄약 수 설정
	//CurrentSMGAmmo = MaxSMGAmmo;
	//CurrentSniperAmmo = MaxSniperAmmo;
	//CurrentShotGunAmmo = MaxShotGunAmmo;

	//스폰한 무기액터클래스 무기장착
	CurrentWeaponActor = GetWorld()->SpawnActor<AMyWeaponActor>(FirstWeaponFactory);
	if (CurrentWeaponActor)
	{
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeaponActor->AttachToComponent(FPSMesh, Rules, TEXT("GripPoint"));
		CurrentWeaponActor->WeaponBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CurrentWeaponActor->SetCamera(fpsCamComp);
		CurrentWeaponActor->SetPlayerBuff(AttackPower);
	}
	AMyWeaponActor* SecondWeaponActor = GetWorld()->SpawnActor<AMyWeaponActor>(SecondWeaponFactory);
	if (SecondWeaponActor)
	{
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		SecondWeaponActor->AttachToComponent(FPSMesh, Rules, TEXT("GripPoint"));
		SecondWeaponActor->WeaponBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SecondWeaponActor->SetCamera(fpsCamComp);
		SecondWeaponActor->SetPlayerBuff(AttackPower);
	}
	AMyWeaponActor* ThirdWeaponActor = GetWorld()->SpawnActor<AMyWeaponActor>(ThirdWeaponFactory);
	if (ThirdWeaponActor)
	{
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		ThirdWeaponActor->AttachToComponent(FPSMesh, Rules, TEXT("GripPoint"));
		ThirdWeaponActor->WeaponBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ThirdWeaponActor->SetCamera(fpsCamComp);
		ThirdWeaponActor->SetPlayerBuff(AttackPower);
	}


	//무기장착슬롯을 생성한다.
	EquippedWeapons.Add(CurrentWeaponActor);
	// GainWeapon(WeaponActor);
	EquippedWeapons.Add(SecondWeaponActor);
	EquippedWeapons.Add(ThirdWeaponActor);

	ChangeWeapon(0);

	gunMeshComp->SetVisibility(false);
}

void ATPSPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(LaunchUpwardTimeHandle);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ResetFrictionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(SniperUITimerHandle);
	/*GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(SMGFIreTimer);*/
	GetWorld()->GetTimerManager().ClearTimer(LoadingHandle);
	GetWorld()->GetTimerManager().ClearTimer(DamagedHandle);
	GetWorld()->GetTimerManager().ClearTimer(GrenadeHandle);
	GetWorld()->GetTimerManager().ClearTimer(DashEffectDeactivateHandle);
	/*GetWorld()->GetTimerManager().ClearTimer(SniperEffectTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(SMGEffectTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ShotGunEffectTimerHandle);*/
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*MyMove();*/

	//PlayerHUD UI업데이트
	UpdateMyHUD();

	//카메라 위치를 머리 위치로 지속 갱신
	FVector newCameraLocation = GetMesh()->GetSocketLocation(TEXT("headSocket"));
	fpsCamComp->SetWorldLocation(newCameraLocation);

}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//마우스 컨트롤 이벤트 처리함수 바인딩
	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if(PlayerInput)
	{
		//컴포넌트에서 입력바인딩을 처리하도록 가상함수 호출
		playerMove->SetupInputBinding(PlayerInput);
		/*PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);*/
	}
	////상하좌우입력 이벤트처리함수 반인딩
	//PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::InputMove);
	////점프입력 이벤트처리함수 바인딩
	//PlayerInput->BindAction(ia_Jump, ETriggerEvent::Triggered, this, &ATPSPlayer::InputJump);
	//시점전환 이벤트처리함수 바인딩
	PlayerInputComponent->BindAction(TEXT("ToggleView"), IE_Pressed, this, &ATPSPlayer::ToggleView);
	//총알발사 이벤트처리함수 바인딩
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::InputFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ATPSPlayer::StopFire);
	//총기교체 이벤트처리함수 바인딩
	PlayerInputComponent->BindAction(TEXT("Weapon1"), IE_Pressed, this, &ATPSPlayer::Weapon1);
	PlayerInputComponent->BindAction(TEXT("Weapon2"), IE_Pressed, this, &ATPSPlayer::Weapon2);
	PlayerInputComponent->BindAction(TEXT("Weapon3"), IE_Pressed, this, &ATPSPlayer::Weapon3);
	//오버랩 무기교체이벤트 처리함수 바인딩
	PlayerInputComponent->BindAction(TEXT("Function"), IE_Pressed, this, &ATPSPlayer::GainWeapon);
	//스나이퍼 조준모드 이벤트처리함수 바인딩
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);
	////달리기입력 이벤트처리함수 바인딩
	//PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ATPSPlayer::InputRun);
	////PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ATPSPlayer::InputRun);
	//대시기능 이벤트처리함수 바인딩
	PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &ATPSPlayer::InputLaunch);
	//수류탄발사 이벤트처리함수 바인딩
	PlayerInputComponent->BindAction(TEXT("Grenade"), IE_Pressed, this, &ATPSPlayer::InputGrenade);
	//재장전 이벤트처리함수 바인딩
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ATPSPlayer::Reload);
}

//로딩후 UI 일괄 AddtoViewport 함수
void ATPSPlayer::ShowMainUI()
{
	if (nullptr != _loadingUI)
	{
		//_loadingUI->RemoveFromParent();
		_loadingUI->SetVisibility(ESlateVisibility::Hidden);
		_takedamageUI->SetVisibility(ESlateVisibility::Visible);
		_crosshairUI->SetVisibility(ESlateVisibility::Visible);
		_playerHUDUI->SetVisibility(ESlateVisibility::Visible);
	}
}

//Player HUD UI 업데이트 함수 추가
void ATPSPlayer::UpdateMyHUD()
{
	if (!_playerHUDUI) return;

	//HUD의 HP Bar와 AMMO Text블록에 접근해 값을 설정
	UProgressBar* HPBar = Cast<UProgressBar>(_playerHUDUI->GetWidgetFromName(TEXT("HPBar")));
	UTextBlock* AmmoText = Cast<UTextBlock>(_playerHUDUI->GetWidgetFromName(TEXT("AmmoText")));
	UTextBlock* MaxAmmoText = Cast<UTextBlock>(_playerHUDUI->GetWidgetFromName(TEXT("MaxAmmoText")));

	//HPText와 MaxHPText 텍스트 블록에 접근해 값을 설정
	UTextBlock* HPText = Cast<UTextBlock>(_playerHUDUI->GetWidgetFromName(TEXT("HPText")));
	UTextBlock* MaxHPText = Cast<UTextBlock>(_playerHUDUI->GetWidgetFromName(TEXT("MaxHPText")));

	if (HPBar)
	{
		//현재 ProgressBar의 퍼센트를 PlayerHP/100으로 바인딩
		HPBar->SetPercent( PlayerHP / PlayerMAXHP );
	}
	if (AmmoText)
	{
		//AmmoText의 값을 현재 장착한 무기의 탄약수로 바인딩
		int32 CurrentAmmo = CurrentWeaponActor->CurrentWeaponAmmo;
		/*switch (CurrentWeaponType)
		{
		case EWeaponType::SMG:
			CurrentAmmo = CurrentSMGAmmo;
			break;
		case EWeaponType::SniperGun:
			CurrentAmmo = CurrentSniperAmmo;
			break;
		case EWeaponType::ShotGun:
			CurrentAmmo = CurrentShotGunAmmo;
			break;
		default:
			break;
		}*/
		AmmoText->SetText(FText::AsNumber(CurrentAmmo));
	}
	if (MaxAmmoText)
	{
		//MaxAmmoText의 값을 현재 장착한 무기의 Max 탄약수로 바인딩
		int32 MaxAmmo = CurrentWeaponActor->MaxWeaponAmmo;
		/*switch (CurrentWeaponType)
		{
		case EWeaponType::SMG:
			MaxAmmo = MaxSMGAmmo;
			break;
		case EWeaponType::SniperGun:
			MaxAmmo = MaxSniperAmmo;
			break;
		case EWeaponType::ShotGun:
			MaxAmmo = MaxShotGunAmmo;
			break;
		default:
			break;
		}*/
		MaxAmmoText->SetText(FText::AsNumber(MaxAmmo));
	}

	//PlayerHP와 PlayerMAXHP값을 HPText와 MaxHPText의 값에 연동
	if (HPText)
	{
		HPText->SetText(FText::AsNumber(PlayerHP));
	}
	if (MaxHPText)
	{
		MaxHPText->SetText(FText::AsNumber(PlayerMAXHP));
	}

}

void ATPSPlayer::GainWeapon()
{
	if (nullptr != OverlappedWeapon)
	{
		// 기존에 무기가 있다면 버린다.
		if (!CurrentWeaponActor->IsA<AMyEmptyWeaponActor>())
		{
			CurrentWeaponActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CurrentWeaponActor->WeaponBoxComp->SetSimulatePhysics(true);
			CurrentWeaponActor->WeaponBoxComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
		}
		//들어온 무기정보로 현재 무기슬롯의 무기를 채운다.
		EquippedWeapons[CurrentWeaponIndex] = OverlappedWeapon;
		CurrentWeaponActor = OverlappedWeapon;
		
		//무기를 Attach해준다.
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeaponActor->AttachToComponent(FPSMesh, Rules, TEXT("GripPoint"));
		CurrentWeaponActor->WeaponBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CurrentWeaponActor->SetCamera(fpsCamComp);
		CurrentWeaponActor->SetPlayerBuff(AttackPower);

		OverlappedWeapon = nullptr;
	}
}

//무기습득 이벤트처리함수
void ATPSPlayer::OnMyOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapped!"));
	//오버랩된 액터가 존재하고, 그 액터가 나 자신이 아니면서, 오버랩된 컴포넌트가 존재하는지 체크
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		OverlappedWeapon = Cast<AMyWeaponActor>(OtherActor);
	}
}

void ATPSPlayer::OnMyOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == OverlappedWeapon)
	{
		OverlappedWeapon = nullptr;
	}
}

//1인칭/3인칭 시점 전환함수 구현
void ATPSPlayer::ToggleView()
{
	//현재의 카메라시점상태 체크 변수를 토글한다.
	bIsFirstPerson = !bIsFirstPerson;

	//각 조건체크 상황에 따라 TPS/FPS카메라를 활성화한다.
	tpsCamComp->SetActive(!bIsFirstPerson);
	fpsCamComp->SetActive(bIsFirstPerson);

	//현재 FPS/TPS 카메라 상태에 따라 캐릭터 메시를 활성화한다.
	if (bIsFirstPerson)
	{
		GetMesh()->SetVisibility(false, true);
		FPSMesh->SetVisibility(true, true);
	}
	else
	{
		GetMesh()->SetVisibility(true, true);
		FPSMesh->SetVisibility(false, true);
	}
}

//피격 데미지 이벤트처리함수 구현
float ATPSPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 데미지 UI를 화면에 표시
	if (nullptr != _takedamageUI)
	{
		//_takedamageUI->SetVisibility(ESlateVisibility::Visible);
		_takedamageUI->AddToViewport(1);
	}

	//플레이어 HP 감소처리
	PlayerHP -= FinalDamage;
	if (PlayerHP <= 0)
	{
		PlayerHP = 0;

		// 게임 멈추기
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		//플레이어가 사망했을때 UI출력
		_sniperUI->SetVisibility(ESlateVisibility::Hidden);
		_crosshairUI->SetVisibility(ESlateVisibility::Hidden);
		_playerHUDUI->SetVisibility(ESlateVisibility::Hidden);

		// _restartUI를 화면에 추가
		if (_restartUI)
		{
			_restartUI->SetVisibility(ESlateVisibility::Visible);
			//_restartUI->AddToViewport();

			// 입력 모드 설정
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				// 마우스 커서 보이게 하기
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(_restartUI->TakeWidget());
				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;
			}
		}
		
	}
	else
	{
		_sniperUI->SetVisibility(ESlateVisibility::Visible);
		_crosshairUI->SetVisibility(ESlateVisibility::Visible);
		_playerHUDUI->SetVisibility(ESlateVisibility::Visible);
	}

	//GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Green, FString::Printf(TEXT("HP : %f"), PlayerHP));

	// 데미지 인디케이터 UI에게 HitLocation값을 전달
	if (DamageCauser)
		DamageIndicator(DamageCauser->GetActorLocation());


	// 일정 시간 후 데미지 UI를 제거
	GetWorldTimerManager().SetTimer(DamagedHandle, FTimerDelegate::CreateLambda([&]()
		{
			if (_takedamageUI)
			{
				//_takedamageUI->SetVisibility(ESlateVisibility::Hidden);
				_takedamageUI->RemoveFromParent();
			}
		}), 1.0f, false);

	return FinalDamage;
}

#pragma region Moving, Dash Event (LaunchCharacter)

////마우스 컨트롤 이벤트처리함수 구현
//void ATPSPlayer::Turn(const struct FInputActionValue& inputValue)
//{
//	float value = inputValue.Get<float>();
//	AddControllerYawInput(value);
//}
//void ATPSPlayer::LookUp(const struct FInputActionValue& inputValue)
//{
//	float value = inputValue.Get<float>();
//	AddControllerPitchInput(value);
//}

////상하좌우이동 입력이벤트 처리함수
//void ATPSPlayer::InputMove(const struct FInputActionValue& inputValue)
//{
//	FVector2D value = inputValue.Get<FVector2D>();
//	//상하입력이벤트처리
//	direction.X = value.X;
//	//좌우입력이벤트처리
//	direction.Y = value.Y;
//}

////Jump 이벤트처리함수 구현
//void ATPSPlayer::InputJump(const struct FInputActionValue& inputValue)
//{
//	Jump();
//}

////이동함수 Move(최적화)
//void ATPSPlayer::MyMove()
//{
//	//절대좌표를 상대좌표로 변경
//	direction = FTransform(GetControlRotation()).TransformVector(direction);
//	//언리얼에서 제공하는 기본 무브먼트 컴포넌트 사용
//	AddMovementInput(direction);
//	direction = FVector::ZeroVector; //초기화
//}

////달리기 이벤트처리 함수 구현
//void ATPSPlayer::InputRun()
//{
//	//달리기 상태를 토글 방식으로 변경
//	bisRunning = !bisRunning;
//
//	//달리기상태에 따라 속도변경
//	if (bisRunning)
//	{
//		GetCharacterMovement()->MaxWalkSpeed = runSpeed;
//	}
//	else
//	{
//		GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
//	}
//
//	
//}

//대쉬 이벤트처리 함수 구현
void ATPSPlayer::InputLaunch()
{
	//만약 지금 대쉬가 가능한 상태라면
	if (bCanLaunch)
	{
		//1. cooldown이 끝날때까지 조건을 false로 변경
		bCanLaunch = false;
		//2. 대쉬하는 동안 마찰력 0인 상태로 변경
		GetCharacterMovement()->GroundFriction = 0.0f;
		//3. 원하는 방향으로 이동하고
		LaunchVelocity();
		//4. 0.01 딜레이 후 UpVector 이동
		GetWorldTimerManager().SetTimer(LaunchUpwardTimeHandle, this, &ATPSPlayer::LaunchUpward, 0.01f, false);
		//5. 1초 딜레이 후 Cooldown 초기화
		GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ATPSPlayer::ResetLaunchCooldown, 1.0f, false);
		//6. 쿨다운 초기화 후 마찰력 복구
		GetWorldTimerManager().SetTimer(ResetFrictionTimerHandle, this, &ATPSPlayer::ResetGroundFriction, 1.0f, false);

		// 대시 시 위아래 흔들림 증가
		//ApplyMeshBobbing(GetWorld()->GetDeltaSeconds(), 4.0f); // 대시 중 강한 흔들림 적용
		//GetWorldTimerManager().SetTimer(BobbingIntensityResetTimerHandle, [this]()
		//	{
		//		// 2.0초 후 흔들림 강도를 원래 상태로 되돌림
		//		ApplyMeshBobbing(GetWorld()->GetDeltaSeconds(), 1.0f);
		//	}, 2.0f, false);

		// 대시 종료 시 나이아가라 이펙트 해제
		GetWorldTimerManager().SetTimerForNextTick([this]()
			{
				if (PlayerDashEffect && PlayerDashEffect->IsActive())
				{
					GetWorldTimerManager().SetTimer(DashEffectDeactivateHandle, [this]()
						{
							PlayerDashEffect->Deactivate();
						}, 1.0f, false);
				}
			});

	}
}

//원하는 방향으로 대쉬 기능 함수 구현
void ATPSPlayer::LaunchVelocity()
{
	//앞으로 이동시킬 Velocity
	//FVector LaunchVelocity = GetActorForwardVector() * 1000; //앞으로만 이동을 원할때
	//원하는 방향으로 이동시킬 Veloicity 변수 선언
	FVector LaunchVelocity = GetVelocity().GetSafeNormal() * 1400;
	//캐릭터를 앞으로 이동
	LaunchCharacter(LaunchVelocity, true, false);

	// 나이아가라 이펙트 스폰
	if (PlayerDashEffect && PlayerDashEffect->GetAsset())
	{
		GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Red, FString::Printf(TEXT("DashEffect : %d"), PlayerDashEffect));
		PlayerDashEffect->Activate(true);
	}
}

//대쉬할떄 위로 살짝 뜨게하는 함수 구현
void ATPSPlayer::LaunchUpward()
{
	//위로 300의 힘으로 이동시킬 변수
	FVector LaunchVelocity = FVector(0, 0, 150);
	LaunchCharacter(LaunchVelocity, false, true);
}

//Cooldown 초기화 함수 구현
void ATPSPlayer::ResetLaunchCooldown()
{
	bCanLaunch = true;
}

//마찰력상태 초기화 함수 구현
void ATPSPlayer::ResetGroundFriction()
{
	GetCharacterMovement()->GroundFriction = OriginalGroundFriction;
}

#pragma endregion

#pragma region Firing Setting

//총알발사처리함수
void ATPSPlayer::InputFire()
{
	//발사 애니메이션 처리
	auto* anim = FPSMesh->GetAnimInstance();
	if (anim)
	{
		anim->Montage_Play(FireMontage);
	}

	if (true)
	{
		CurrentWeaponActor->WeaponAttack();
		return ;
	}
	//UE_LOG(LogTemp, Warning, TEXT("InputFire CurrentWeaponType : %d"), CurrentWeaponType);
	//bIsFiring = true;//발사 상태 체크

	//Switch구문 이용하여 총기발사처리
	//현재 무기타입에 따라 탄약수 확인 후 발사처리
	//switch (CurrentWeaponType)
	//{
	//case EWeaponType::GrenadeGun:
	//{
	//	// 총구 화염 파티클 생성
	//	if (MuzzleFlash)
	//	{
	//		FTransform MuzzleTransform = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
	//	}

	//	//총 스켈레탈메시 컴포넌트의 총구위치 정보를 가져온다.
	//	FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	//	//총구위치에 총알공장이 위치하고, 거기에서 총알을 생성한다.
	//	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	//	if (Bullet)
	//	{
	//		Bullet->Damage = BaseGrenadeGunDamage * AttackPower;
	//	}

	//	//발사상태 설정
	//	bIsFiring = true;
	//}
	//break;

	//case EWeaponType::SniperGun:
	//{
	//	if (CurrentSniperAmmo > 0)
	//	{
	//		HandleSniperFire();
	//		CurrentSniperAmmo--;
	//		UE_LOG(LogTemp, Warning, TEXT("AMMO : %d"), CurrentSniperAmmo);
	//		GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Blue, FString::Printf(TEXT("AMMO : %d"), CurrentSniperAmmo));

	//		if (CurrentSniperAmmo == 0)
	//		{
	//			Reload();
	//		}
	//	}
	//}
	//break;

	//case EWeaponType::SMG:
	//{
	//	GetWorldTimerManager().SetTimer(SMGFIreTimer, this, &ATPSPlayer::HandleSMGFire, SMGFireRate, true);
	//}
	//break;

	//case EWeaponType::ShotGun:
	//{
	//	if (CurrentShotGunAmmo > 0)
	//	{
	//		HandleShotGunFire();
	//		CurrentShotGunAmmo--;
	//		UE_LOG(LogTemp, Warning, TEXT("AMMO : %d"), CurrentShotGunAmmo);
	//		GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Blue, FString::Printf(TEXT("AMMO : %d"), CurrentShotGunAmmo));

	//		if (CurrentShotGunAmmo == 0)
	//		{
	//			Reload();
	//		}
	//	}
	//}
	//break;

	//default:
	//	break;
	//}
}

//무기탄창 재장전 함수
void ATPSPlayer::Reload()
{
	
	//재장전 애니메이션 처리
	auto* anim = FPSMesh->GetAnimInstance();
	if (anim)
	{
		anim->Montage_Play(ReloadMontage);
	}
	CurrentWeaponActor->WeaponReload();
   
	// (재장전애니메이션시간)1.0초 후에 탄약을 채우고 발사 가능 상태로 설정
	//GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, [this]()
	//	{
	//		// 현재 무기 타입에 따라 재장전 처리
	//		switch (CurrentWeaponType)
	//		{
	//		case EWeaponType::SMG:
	//			CurrentSMGAmmo = MaxSMGAmmo;
	//			break;
	//		case EWeaponType::SniperGun:
	//			CurrentSniperAmmo = MaxSniperAmmo;
	//			break;
	//		case EWeaponType::ShotGun:
	//			CurrentShotGunAmmo = MaxShotGunAmmo;
	//			break;
	//		default:
	//			break;
	//		}
	//	}, 1.0f, false);
}

#pragma endregion

#pragma region Weapon Change Logic Setting

//무기변경 로직 함수
void ATPSPlayer::ChangeWeapon(int32 slot)
{
	
	//현재 무기인덱스를 slot으로 변경
	CurrentWeaponIndex = slot;
	//방어코드
	if (nullptr != CurrentWeaponActor)
	{
		//현재 무기의 가시성 해제
		CurrentWeaponActor->GunMesh->SetVisibility(false);
	}
	//현재 무기를 slot에 해당하는 무기로 변경
	CurrentWeaponActor = EquippedWeapons[slot];
	//방어코드
	if (nullptr != CurrentWeaponActor)
	{
		//현재 무기의 가시성 설정
		CurrentWeaponActor->GunMesh->SetVisibility(true);
	}

	////전달된 슬롯 인덱스가 무기 슬롯의 갯수보다 작은지 체크(false면 동작안하고 함수 종료)
	//if (slot < WeaponSlots.Num())
	//{
	//	//주어진 슬롯인덱스의 무기를 현재무기타입으로 설정
	//	CurrentWeaponType = WeaponSlots[slot];
	//	//스나이퍼UI사용여부 체크를 위해 현재무기타입이 저격총타입인지 체크 
	//	bUsingSniperGun = CurrentWeaponType == EWeaponType::SniperGun;

		//FPS모드인 경우 총기 메시를 FPS전용 메시에 부착(미사용)
		//if (bIsFirstPerson)
		//{
		//	gunMeshComp->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	sniperGunComp->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	SMGMesh->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	ShotGunMesh->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		//}
		////TPS모드인 경우 총기 메시를 TPS메시에 부착(미사용)
		//else
		//{
		//	gunMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	sniperGunComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	SMGMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	ShotGunMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		//}

	//	//무기메시 가시성 조정
	//	gunMeshComp->SetVisibility(CurrentWeaponType == EWeaponType::GrenadeGun);
	//	sniperGunComp->SetVisibility(CurrentWeaponType == EWeaponType::SniperGun);
	//	SMGMesh->SetVisibility(CurrentWeaponType == EWeaponType::SMG);
	//	ShotGunMesh->SetVisibility(CurrentWeaponType == EWeaponType::ShotGun);
	//}
}

//1번 무기 선택 함수
void ATPSPlayer::Weapon1()
{
	ChangeWeapon(0);
}
//2번 무기 선택 함수
void ATPSPlayer::Weapon2()
{
	ChangeWeapon(1);
}
//3번 무기 선택 함수
void ATPSPlayer::Weapon3()
{
	ChangeWeapon(2);
}


#pragma endregion

#pragma region Weapon Settings

//스나이퍼 발사처리함수(LineTrace 활용)
//void ATPSPlayer::HandleSniperFire()
//{
//	// 카메라 컴포넌트 선택
//	UCameraComponent* activeCamComp = bIsFirstPerson ? fpsCamComp : tpsCamComp;
//
//	//LineTrace의 시작위치(활성화된 카메라 기준)
//	FVector startPos = activeCamComp->GetComponentLocation();
//	//LineTrace의 종료위치(활성화된 카메라 정중앙에서 전방으로 N만큼)
//	FVector endPos = startPos + (activeCamComp->GetForwardVector() * 15000);
//	//LineTrace의 충돌정보를 담을 변수
//	FHitResult hitInfo;
//	//충돌옵션 설정변수
//	FCollisionQueryParams params;
//	//자기자신(플레이어)는 충돌에서 제외
//	params.AddIgnoredActor(this);
//	//Channel 필터를 이용한 LineTrace 충돌 검출
//	//(충돌정보, 시작위치, 종료위치, 검출채널, 충돌옵션)
//	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
//	
//	// 나이아가라 시스템 스폰
//	UNiagaraComponent* TrailNSC = nullptr;
//	if (SniperTrail)
//	{
//		FVector TraceVector = hitInfo.ImpactPoint - startPos;
//		FVector SpawnPos = startPos + (TraceVector * 0.1f); // 10% 지점에서 스폰
//		FRotator TrailRotation = TraceVector.Rotation();
//		TrailNSC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SniperTrail, SpawnPos, TrailRotation);
//	}
//
//	//LineTrace가 부딪쳤을때(bHit가 true일 때)
//	if (bHit)
//	{
//		//충돌처리 -> 총알 파편효과 재생
//		//총알파편효과 트랜스폼 변수 선언
//		FTransform bulletTrans;
//		//부딪친 위치 할당
//		bulletTrans.SetLocation(hitInfo.ImpactPoint);
//		//총알 파편 효과 인스턴스 생성
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);
//
//		//피격된 액터에 데미지 적용
//		UGameplayStatics::ApplyPointDamage(hitInfo.GetActor(), BaseSniperGunDamage * AttackPower, activeCamComp->GetForwardVector(), hitInfo, GetController(), this, nullptr);
//
//		//피격물체 날려보내기 구현
//		auto hitComp = hitInfo.GetComponent();
//		//1. 만약 맞은 컴포넌트에 물리가 적용되어있다면
//		if (hitComp && hitComp->IsSimulatingPhysics())
//		{
//			//2. 날려버릴 힘의 정도와 방향을 정해주고
//			FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 50000;
//			//3. 그 방향으로 날려보낸다.
//			hitComp->AddForce(force);
//		}
//
//		// bHit가 됐을때 0.4초 지연후 나이아가라 시스템 비활성화
//		if (TrailNSC)
//		{
//			GetWorld()->GetTimerManager().SetTimer(SniperEffectTimerHandle, [TrailNSC]()
//				{
//					if (TrailNSC)
//					{
//						TrailNSC->Deactivate();
//					}
//				}, 0.4f, false);
//		}
//	}
//	else
//	{
//		
//	}
//
//	// 총구 화염 파티클 생성
//	if (MuzzleFlash)
//	{
//		FTransform MuzzleTransform = sniperGunComp->GetSocketTransform(TEXT("FirePosition"));
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
//	}
//
//	//발사상태 설정
//	bIsFiring = true;
//}

//스나이퍼 조준 함수 구현
void ATPSPlayer::SniperAim()
{
	// 스나이퍼건 모드가 아니라면 처리하지 않는다.
	/*if (bUsingSniperGun == false)
	{
		return;
	}*/
	if (CurrentWeaponActor->bSniperMode == false)
	{
		return ;
	}

	// Pressed 입력 처리
	if (!bSniperAim)
	{
		// 1. 스나이퍼 조준 모드 활성화
		bSniperAim = true;

		if (bIsFirstPerson == true)
		{
			// 2. 0.28초 후에 스나이퍼 조준 UI 활성화 (1인칭 모드)
			GetWorldTimerManager().SetTimer(SniperUITimerHandle, FTimerDelegate::CreateLambda([this]()
				{
					if (bSniperAim)
					{
						_sniperUI->AddToViewport(0);
						//_crosshairUI->RemoveFromParent();
						//_sniperUI->SetVisibility(ESlateVisibility::Visible);
						_crosshairUI->SetVisibility(ESlateVisibility::Hidden);
						fpsCamComp->SetFieldOfView(45.0f);
						//스나이퍼 총 메시 가시성 해제
						CurrentWeaponActor->GunMesh->SetVisibility(false);
					}
				}), 0.28f, false);

			// 타임라인 줌 모드 시작
			StartZoom();
		}
		else
		{
			// 2. 스나이퍼 조준 UI 등록 (3인칭 모드)
			//_sniperUI->AddToViewport(0);
			//_crosshairUI->RemoveFromParent();
			_sniperUI->SetVisibility(ESlateVisibility::Visible);
			_crosshairUI->SetVisibility(ESlateVisibility::Hidden);
			// 카메라의 시야각 Field Of View 설정
			tpsCamComp->SetFieldOfView(45.0f);
			//스나이퍼 총 메시 가시성 해제
			CurrentWeaponActor->GunMesh->SetVisibility(false);
		}
	}
	// Released 입력 처리
	else
	{
		// 1. 스나이퍼 조준 모드 비활성화
		bSniperAim = false;

		// 타이머 중지
		GetWorldTimerManager().ClearTimer(SniperUITimerHandle);

		// 2. 스나이퍼 조준 UI 화면에서 제거
		_sniperUI->RemoveFromParent();
		//_crosshairUI->AddToViewport(2);
		//_sniperUI->SetVisibility(ESlateVisibility::Hidden);
		_crosshairUI->SetVisibility(ESlateVisibility::Visible);

		// 3. 카메라 시야각 원래대로 복원
		fpsCamComp->SetFieldOfView(90.0f);
		tpsCamComp->SetFieldOfView(90.0f);
		// 4. 일반 조준 UI 등록
		// 5. 스나이퍼 총 메시 가시성 복원
		CurrentWeaponActor->GunMesh->SetVisibility(true);

		if (bIsFirstPerson)
		{
			// 타임라인 줌 모드 종료
			EndZoom();
		}
	}
}

//타임라인 줌모드 시작-종료함수 구현
void ATPSPlayer::StartZoom()
{
	if (ZoomTimeLine->IsPlaying())
	{
		ZoomTimeLine->Stop();
	}
	ZoomTimeLine->PlayFromStart();
	
}
void ATPSPlayer::EndZoom()
{
	if (ZoomTimeLine->IsPlaying())
	{
		ZoomTimeLine->Stop();
	}
	ZoomTimeLine->Reverse();
}

//스나이퍼 조준모드 타임라인 함수 구현
void ATPSPlayer::ZoomTimeLineProgress(float value)
{
	// 스나이퍼건 모드가 아니거나 3인칭 모드라면 처리하지 않는다.
	if (CurrentWeaponActor->bSniperMode == false || bIsFirstPerson == false)
	{
		return;
	}

	// 무기 위치와 회전 변경
	FVector NewLocation = FMath::Lerp(OriginalGunTransform.GetLocation(), TargetGunTransform.GetLocation(), value);
	FRotator NewRotation = FMath::Lerp(OriginalGunTransform.GetRotation().Rotator(), TargetGunTransform.GetRotation().Rotator(), value);
	CurrentWeaponActor->GunMesh->SetRelativeLocationAndRotation(NewLocation, NewRotation);

	// 카메라의 시야각 Field Of View 설정
	tpsCamComp->SetFieldOfView(FMath::Lerp(90.0f, 45.0f, value));
}

//수류탄발사 함수 구현
void ATPSPlayer::InputGrenade()
{
	if (GrenadeFactory)
	{
		//1. 수류탄을 던질 위치 계산(캐릭터 위치에서 약간 뒤로 조정)
		FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 50);
		//2. 캐릭터의 회전방향을 얻어둠
		FRotator SpawnRotation = GetControlRotation();
		//3. GrenadeFactory를 이용해서 수류탄 스폰
		AGrenade* Grenade = GetWorld()->SpawnActor<AGrenade>(GrenadeFactory, SpawnLocation, SpawnRotation);

		if (Grenade)
		{
			//4. 던지는 방향을 설정할 벡터얻기
			FVector LaunchDirection = GetActorForwardVector() * 0.7 + GetActorUpVector() * 0.5;
			//5. 수류탄의 움직임을 설정하기 위해 UProjectileMovement의 Velocity속성을 사용
			//수류탄이 던져지는 방향과 속력에 대한 정의
			Grenade->GetProjectileMovementComponent()->Velocity = LaunchDirection * ThrowingForce;
			// 수류탄 데미지 적용
			Grenade->Damage = BaseGrenadeDamage * AttackPower; // 수류탄 데미지 설정
			Grenade->ExplosionRadius = 300.0f; // 수류탄 폭발 반경 설정 (필요에 따라 조정)

			// 일정 시간 후 폭발하도록 타이머 설정
			GetWorldTimerManager().SetTimer(GrenadeHandle, FTimerDelegate::CreateLambda([Grenade]()
				{
					Grenade->Explode();
				}), 1.8f, false); // 1.8초 후 폭발

		}
	}
}

//SMG 발사중지 처리함수
void ATPSPlayer::StopFire()
{
	////발사상태 체크
	//bIsFiring = false;
	////SMG발사중지처리
	//GetWorldTimerManager().ClearTimer(SMGFIreTimer);
	CurrentWeaponActor->WeaponStopAttack();
}

////SMG 발사처리함수(LineTrace활용)
//void ATPSPlayer::HandleSMGFire()
//{
//	UE_LOG(LogTemp, Warning, TEXT("AMMO : %d"), CurrentSMGAmmo);
//	GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Blue, FString::Printf(TEXT("AMMO : %d"), CurrentSMGAmmo));
//
//	if (CurrentSMGAmmo > 0)
//	{
//		CurrentSMGAmmo = CurrentSMGAmmo - 1;
//	}
//	//탄약수가 0이면 발사 중지 및 재장전
//	if (CurrentSMGAmmo <= 0)
//	{
//		StopFire();
//		Reload();
//		return;
//	}
//
//	// 카메라 컴포넌트 선택
//	UCameraComponent* activeCamComp = bIsFirstPerson ? fpsCamComp : tpsCamComp;
//
//	//LineTrace의 시작위치(활성화된 카메라 위치)
//	FVector startPos = activeCamComp->GetComponentLocation();
//	//LineTrace의 종료위치(활성화된 카메라 정중앙에서 전방으로 SMGRange만큼)
//	FVector endPos = startPos + (activeCamComp->GetForwardVector() * SMGRange);
//	//LineTrace의 충돌정보를 담을 변수
//	FHitResult hitInfo;
//	//충돌옵션 설정변수
//	FCollisionQueryParams params;
//	//자기자신(플레이어)는 충돌에서 제외
//	params.AddIgnoredActor(this);
//	//Channel 필터를 이용한 LineTrace 충돌 검출
//	//(충돌정보, 시작위치, 종료위치, 검출채널, 충돌옵션)
//	FHitResult HitResult;
//	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
//
//	// 나이아가라 시스템 스폰
//	UNiagaraComponent* TrailNSC = nullptr;
//	if (SMGTrail)
//	{
//		FVector TraceVector = hitInfo.ImpactPoint - startPos;
//		FVector SpawnPos = startPos + (TraceVector * 0.5f); // 50% 지점에서 스폰
//		FRotator TrailRotation = TraceVector.Rotation();
//		TrailNSC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SniperTrail, SpawnPos, TrailRotation);
//	}
//
//	//LineTrace가 부딪쳤을때(bHit가 true일 때)
//	if (bHit)
//	{
//		//충돌처리 -> 총알 파편효과 재생
//		//총알파편효과 트랜스폼 변수 선언
//		FTransform bulletTrans;
//		//부딪친 위치 할당
//		bulletTrans.SetLocation(hitInfo.ImpactPoint);
//		//총알 파편 효과 인스턴스 생성
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);
//
//		//적중한 액터에 데미지 적용(플레이어 ATK 반영)
//		UGameplayStatics::ApplyPointDamage(hitInfo.GetActor(), BaseSMGDamage * AttackPower, activeCamComp->GetForwardVector(), HitResult, GetController(), this, nullptr);
//
//		//피격물체 날려보내기 구현
//		auto hitComp = hitInfo.GetComponent();
//		//1. 만약 맞은 컴포넌트에 물리가 적용되어있다면
//		if (hitComp && hitComp->IsSimulatingPhysics())
//		{
//			//2. 날려버릴 힘의 정도와 방향을 정해주고
//			FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 5000;
//			//3. 그 방향으로 날려보낸다.
//			hitComp->AddForce(force);
//		}
//
//		// bHit가 됐을때 0.3초 지연후 나이아가라 시스템 비활성화
//		if (TrailNSC)
//		{
//			GetWorld()->GetTimerManager().SetTimer(SMGEffectTimerHandle, [TrailNSC]()
//				{
//					if (TrailNSC)
//					{
//						TrailNSC->Deactivate();
//					}
//				}, 0.3f, false);
//		}
//		
//	}
//	else
//	{
//		
//	}
//
//	// 총구 화염 파티클 생성
//	if (MuzzleFlash)
//	{
//		FTransform MuzzleTransform = SMGMesh->GetSocketTransform(TEXT("FirePosition"));
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
//	}
//
//	//발사상태 설정
//	bIsFiring = true;
//}

////ShotGun 발사처리함수(LineTrace 활용)
//void ATPSPlayer::HandleShotGunFire()
//{
//	// 카메라 컴포넌트 선택
//	UCameraComponent* activeCamComp = bIsFirstPerson ? fpsCamComp : tpsCamComp;
//
//	FVector startPos = activeCamComp->GetComponentLocation();
//	FVector forwardVector = activeCamComp->GetForwardVector();
//
//	//원뿔형으로 뻗어나가도록 각도 조정
//	float horizontalAngleStep = 4.0f; //수평각도 간격
//	float verticalAngleStep = 4.0f;   //수직각도 간격
//	int32 numTraces = 8; //라인트레이스 갯수
//
//	//10개의 라인트레이스선이 뻗어나가도록 생성(샷건모드)
//	for (int32 i = 0; i < numTraces; i++)
//	{
//		//무작위각도를 생성하여 원뿔형태로 퍼지도록 함
//		float randomYaw = FMath::RandRange(-horizontalAngleStep, horizontalAngleStep);
//		float randomPitch = FMath::RandRange(-verticalAngleStep, verticalAngleStep);
//
//		//각도를 회전벡터로 변환
//		FRotator randomRotator = FRotator(randomPitch, randomYaw, 0.0f);
//		FVector rotatedVector = randomRotator.RotateVector(forwardVector);
//		FVector endPos = startPos + (rotatedVector * ShotGunRange);
//		
//		//충돌정보 저장
//		FHitResult hitInfo;
//		FCollisionQueryParams params;
//		//나 자신은 충돌 무시
//		params.AddIgnoredActor(this);
//
//		//라인트레이스실행 및 설정에 따른 충돌 및 데미지 적용
//		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
//
//		// 나이아가라 시스템 스폰
//		UNiagaraComponent* TrailNSC = nullptr;
//		if (ShotGunTrail)
//		{
//			FVector TraceVector = hitInfo.ImpactPoint - startPos;
//			FVector SpawnPos = startPos + (TraceVector * 0.7f); // 70% 지점에서 스폰
//			FRotator TrailRotation = TraceVector.Rotation();
//			TrailNSC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SniperTrail, SpawnPos, TrailRotation);
//		}
//
//		if (bHit)
//		{
//			// 충돌 처리 -> 총알 파편 효과 재생
//			FTransform bulletTrans;
//			bulletTrans.SetLocation(hitInfo.ImpactPoint);
//			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);
//
//			// 적중한 액터에 데미지 적용
//			UGameplayStatics::ApplyPointDamage(hitInfo.GetActor(), BaseShotGunDamage * AttackPower, rotatedVector, hitInfo, GetController(), this, nullptr);
//
//
//			// 피격 물체 날려보내기 구현
//			auto hitComp = hitInfo.GetComponent();
//			if (hitComp && hitComp->IsSimulatingPhysics())
//			{
//				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 3000;
//				hitComp->AddForce(force);
//			}
//
//			// bHit가 됐을때 0.2초 지연후 나이아가라 시스템 비활성화
//			if (TrailNSC)
//			{
//				GetWorld()->GetTimerManager().SetTimer(ShotGunEffectTimerHandle, [TrailNSC]()
//					{
//						if (TrailNSC)
//						{
//							TrailNSC->Deactivate();
//						}
//					}, 0.2f, false);
//			}
//		}
//		//샷건 반동 적용
//		/*ApplyRecoil(FVector(-30.0f, 0.0f, -30.0f), FRotator(-15.0f, 0.0f, 0.0f), 0.15f);*/
//		//발사상태 설정
//		bIsFiring = true;
//
//		// 총구 화염 파티클 생성
//		if (MuzzleFlash)
//		{
//			FTransform MuzzleTransform = ShotGunMesh->GetSocketTransform(TEXT("FirePosition"));
//			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
//		}
//
//		// 디버그 라인 그리기
//		//DrawDebugLine(GetWorld(), startPos, endPos, bHit ? FColor::Green : FColor::Red, false, 1, 0, 1);
//	}
//}

#pragma endregion

#pragma region THE GRAVE

////애니메이션 적용으로 반동효과 관련 내용 제거
//카메라 반동 업데이트
/*UpdateRecoil(DeltaTime);*/

//평소 기본 흔들림 강도 적용
//ApplyMeshBobbing(GetWorld()->GetDeltaSeconds(), 1.0f);

//유탄발사반동 적용
/*ApplyRecoil(FVector(-6.0f, 0.0f, -6.0f), FRotator(-3.0f, 0.0f, 0.0f), 0.1f);*/

//스나이퍼 반동 적용
/*ApplyRecoil(FVector(-20.0f, 0.0f, -20.0f), FRotator(-8.0f, 0.0f, 0.0f), 0.2f);*/

//SMG 발사반동 적용
/*ApplyRecoil(FVector(-20.0f, 0.0f, -20.0f), FRotator(-0.40f, 0.0f, 0.0f), 0.05f);*/

//총기반동처리함수(애니메이션 대체)
//void ATPSPlayer::ApplyRecoil(FVector NewRecoilOffset, FRotator NewRecoilRotation, float RecoilDuration)
//{
//	// 발사 시작 시 카메라의 트랜스폼 정보 저장
//	if (bIsFirstPerson)
//	{
//		InitialCameraTransform = fpsCamComp->GetComponentTransform();
//	}
//	else
//	{
//		InitialCameraTransform = tpsCamComp->GetComponentTransform();
//	}
//
//	//반동값 설정
//	RecoilOffset = NewRecoilOffset;
//	RecoilRotation = NewRecoilRotation;
//	RecoilDuration = 0.1f;
//
//	//일정시간 후에 반동을 재설정하도록 타이머 세팅
//	GetWorldTimerManager().SetTimer(RecoilTimerHandle, this, &ATPSPlayer::ResetRecoil, RecoilDuration, false);
//}

//반동 애니메이션 재설정 함수(애니메이션 대체)
//void ATPSPlayer::ResetRecoil()
//{
//	// 반동 초기화
//	RecoilOffset = FVector::ZeroVector;
//	RecoilRotation = FRotator::ZeroRotator;
//
//	// 발사 상태 초기화
//	bIsFiring = false;
//
//	// 무기 메시의 위치와 회전 초기화
//	if (CurrentWeaponType == EWeaponType::SniperGun)
//	{
//		sniperGunComp->SetRelativeLocation(FVector(-22, 55, 120));
//		sniperGunComp->SetRelativeRotation(FRotator::ZeroRotator);
//	}
//	else if (CurrentWeaponType == EWeaponType::ShotGun)
//	{
//		ShotGunMesh->SetRelativeLocation(FVector(-14, 52, 120));
//		ShotGunMesh->SetRelativeRotation(FRotator::ZeroRotator);
//	}
//	else if (CurrentWeaponType == EWeaponType::SMG)
//	{
//		//0.2초마다 무기 메시위치를 복원하도록 설정
//		GetWorldTimerManager().SetTimer(SMGResetTimerHandle, this, &ATPSPlayer::ResetSMGPosition, 0.1f, true);
//
//		SMGMesh->SetRelativeLocation(FVector(-14, 52, 130));
//		SMGMesh->SetRelativeRotation(FRotator::ZeroRotator);
//	}
//	else if (CurrentWeaponType == EWeaponType::GrenadeGun)
//	{
//		gunMeshComp->SetRelativeLocation(FVector(-14, 52, 120));
//		gunMeshComp->SetRelativeRotation(FRotator::ZeroRotator);
//	}
//
//	// 카메라의 트랜스폼 정보를 초기 값으로 복원
//	if (bIsFirstPerson)
//	{
//		fpsCamComp->SetWorldTransform(InitialCameraTransform);
//	}
//	else
//	{
//		//tpsCamComp->SetWorldTransform(InitialCameraTransform);
//	}
//}

//반동 애니메이션 업데이트 함수(애니메이션 대체)
//void ATPSPlayer::UpdateRecoil(float DeltaTime)
//{
//	if (bIsFiring)
//	{
//		//카메라 위치와 회전에 반동값 적용
//		if (bIsFirstPerson)
//		{
//			//FPS카메라에 반동 적용
//			APlayerController* PlayerController = GetController<APlayerController>();
//			if (PlayerController)
//			{
//				// 컨트롤러를 통해 카메라 반동 적용
//				PlayerController->AddPitchInput(RecoilRotation.Pitch * 1.5f * DeltaTime);
//				PlayerController->AddYawInput(RecoilRotation.Yaw * 1.5f * DeltaTime);
//			}
//		}
//		//else
//		//{
//		//	// TPS 카메라에 반동 적용
//		//	APlayerController* PlayerController = GetController<APlayerController>();
//		//	if (PlayerController)
//		//	{
//		//		// 컨트롤러를 통해 카메라 반동 적용
//		//		PlayerController->AddPitchInput(RecoilRotation.Pitch * DeltaTime);
//		//		PlayerController->AddYawInput(RecoilRotation.Yaw * DeltaTime);
//		//	}
//		//}
//		// 무기 메시의 위치와 회전에 반동 값 적용
//		if (CurrentWeaponType == EWeaponType::SniperGun)
//		{
//			FVector NewLocation = sniperGunComp->GetRelativeLocation() + (RecoilOffset * DeltaTime);
//			FRotator NewRotation = sniperGunComp->GetRelativeRotation() + (RecoilRotation * DeltaTime);
//			sniperGunComp->SetRelativeLocation(NewLocation);
//			sniperGunComp->SetRelativeRotation(NewRotation);
//		}
//		else if (CurrentWeaponType == EWeaponType::ShotGun)
//		{
//			FVector NewLocation = ShotGunMesh->GetRelativeLocation() + (RecoilOffset * DeltaTime);
//			FRotator NewRotation = ShotGunMesh->GetRelativeRotation() + (RecoilRotation * DeltaTime);
//			ShotGunMesh->SetRelativeLocation(NewLocation);
//			ShotGunMesh->SetRelativeRotation(NewRotation);
//		}
//		else if (CurrentWeaponType == EWeaponType::SMG)
//		{
//			//FVector NewLocation = SMGMesh->GetRelativeLocation() + (RecoilOffset * DeltaTime);
//			//FRotator NewRotation = SMGMesh->GetRelativeRotation() + (RecoilRotation * DeltaTime);
//			//SMGMesh->SetRelativeLocation(NewLocation);
//			//SMGMesh->SetRelativeRotation(NewRotation);
//		}
//		else if (CurrentWeaponType == EWeaponType::GrenadeGun)
//		{
//			FVector NewLocation = gunMeshComp->GetRelativeLocation() + (RecoilOffset * DeltaTime);
//			FRotator NewRotation = gunMeshComp->GetRelativeRotation() + (RecoilRotation * DeltaTime);
//			gunMeshComp->SetRelativeLocation(NewLocation);
//			gunMeshComp->SetRelativeRotation(NewRotation);
//		}
//	}
//}

//자동발사되는 SMG의 경우 별도 반동초기화(애니메이션 대체)
//void ATPSPlayer::ResetSMGPosition()
//{
//	SMGMesh->SetRelativeLocation(FVector(-14, 52, 130));
//	SMGMesh->SetRelativeRotation(FRotator::ZeroRotator);
//}

////애니메이션 적용으로 카메라흔들림 효과 관련 내용 제거
//FPS메시의 초기 위치 저장(메시흔들림 체크용도)
/*InitialMeshLocation = FPSMesh->GetRelativeLocation();*/

/*GetWorld()->GetTimerManager().ClearTimer(RecoilTimerHandle);
GetWorld()->GetTimerManager().ClearTimer(SMGResetTimerHandle);*/

//이동시 메시흔들림 함수 호출
/*ApplyMeshBobbing(DeltaTime, 1.0f);*/

//GetWorld()->GetTimerManager().ClearTimer(BobbingIntensityResetTimerHandle);

// 메시 흔들림 함수 구현
//void ATPSPlayer::ApplyMeshBobbing(float DeltaTime, float BobbingIntensity)
//{
//	// 총기 발사 중에는 흔들림 미적용
//	if (bIsFiring)
//	{
//		//초기 메시 위치로 재조정
//		FPSMesh->SetRelativeLocation(InitialMeshLocation);
//		return;
//	}
//	// 흔들림 속도와 크기 설정
//	float BobbingSpeed = 3.0f; // 흔들림 속도
//	float BobbingAmount = 0.08f * BobbingIntensity; // 흔들림 크기
//
//	// 현재 메시 위치 가져오기
//	FVector NewLocation = FPSMesh->GetRelativeLocation();
//
//	// Z 축 방향으로 사인파를 이용한 흔들림 적용
//	NewLocation.Z += FMath::Sin(GetWorld()->TimeSeconds * BobbingSpeed) * BobbingAmount;
//
//	// 새로운 위치 설정
//	FPSMesh->SetRelativeLocation(NewLocation);
//}

////현재 무기를 바닥에 드랍하는 함수
//void ATPSPlayer::DropCurrentWeapon()
//{
//	//현재 들고있는 무기상태를 담을 변수 선언
//	USkeletalMeshComponent* CurrentWeaponMesh = nullptr;
//	//현재 무기 종류에 따른 드랍할 Mesh상태 결정
//	switch (CurrentWeaponType)
//	{
//	case EWeaponType::GrenadeGun:
//		CurrentWeaponMesh = gunMeshComp;
//		break;
//	case EWeaponType::SniperGun:
//		CurrentWeaponMesh = sniperGunComp;
//		break;
//	case EWeaponType::SMG:
//		CurrentWeaponMesh = SMGMesh;
//		break;
//	case EWeaponType::ShotGun:
//		CurrentWeaponMesh = ShotGunMesh;
//		break;
//	}
//	//현재 무기 슬롯인덱스를 찾아 Empty 상태로 전환
//	int idx = -1;
//	for (int i = 0; i < WeaponSlots.Num(); i++)
//	{
//		//무기타입과 일치하는 슬롯의 idx값 반환
//		if (CurrentWeaponType == WeaponSlots[i])
//		{
//			idx = i;
//			break;
//		}
//	}
//	//반환된 idx값의 슬롯을 empty로 전환
//	WeaponSlots[idx] = EWeaponType::Empty;
//
//	if (CurrentWeaponMesh)
//	{
//		//플레이어에게서 떨어트리기
//		CurrentWeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
//		//충돌체 설정을 살려둬서 바닥에 남아있게하기
//		CurrentWeaponMesh->SetSimulatePhysics(true);
//		CurrentWeaponMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
//	}
//}
//
////바닥에 떨어진 무기를 습득하는 함수
//void ATPSPlayer::PickupWeapon(USkeletalMeshComponent* WeaponMesh, EWeaponType NewWeaponType)
//{
//	//일단 갖고있던 무기가 버려진 후(무기동시소지 방지)
//	DropCurrentWeapon();
//
//	//새로운 무기를 현재 무기로 설정
//	CurrentWeaponType = NewWeaponType;
//	//새로운 무기를 정확한 위치에 플레이어에게 붙이기
//	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket");
//	//습득한 무기의 충돌체 설정 제거(플레이어와 충돌방지)
//	WeaponMesh->SetSimulatePhysics(false);
//	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
//
//
//	//현재 무기 슬롯에 새로운 무기를 등록
//	int idx = -1;
//	for (int i = 0; i < WeaponSlots.Num(); i++)
//	{
//		//무기슬롯에 Empty인 idx값을 찾아서 반환
//		if (WeaponSlots[i] == EWeaponType::Empty)
//		{
//			idx = i;
//			break;
//		}
//	}
//	//반환된 idx값에 새로운 무기타입 등록
//	WeaponSlots[idx] = NewWeaponType;
//	//무기변경함수를 호출해 새로운 무기의 idx값 전달 후 장착
//	ChangeWeapon(idx); //인덱스 값 전달
//}
//
//

#pragma endregion