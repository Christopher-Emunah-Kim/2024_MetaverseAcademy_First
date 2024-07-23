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
#include "Enemy/Enemy.h"

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

	//스나이퍼모드에서 타임라인 삽입
	//타임라인 컴포넌트 초기화
	ZoomTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("ZoomTimeLine"));
	//무기 위치 및 회전 초기화
	OriginalGunTransform = FTransform::Identity;
	//원하는 목표 위치 및 회전으로 설정
	TargetGunTransform = FTransform(FRotator(7.0f, 5.0f, 3.0f), FVector(15.0f, -70.0f, 23.0f)); 
     

#pragma endregion
	
#pragma region Movement Settings

	//2단 점프 구현
	JumpMaxCount = 2;
	//초기 상태를 대시 가능으로 설정
	bCanLaunch = true;
	//기본마찰력상태 변수 초기화
	OriginalGroundFriction = GetCharacterMovement()->GroundFriction;

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
	BaseGrenadeDamage = 150.0f;

	//수류탄 기본설정
	ThrowingForce = 800.0f;

#pragma endregion

}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

#pragma region UI Setting

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

	// 스나이퍼 UI 위젯 인스턴스 생성
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// 타임라인 곡선이 설정되어있는지 확인
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

#pragma endregion

#pragma region Enhanced Input Setting

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
#pragma endregion

	//무기 오버랩이벤트 처리함수 바인딩
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::OnMyOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ATPSPlayer::OnMyOverlapEnd);

	//초기 플레이어 시점 설정
	tpsCamComp->SetActive(!bIsFirstPerson);
	fpsCamComp->SetActive(bIsFirstPerson);

	//초기 시점 설정에 따라 메시 가시성 설정
	GetMesh()->SetVisibility(!bIsFirstPerson, true);
	FPSMesh->SetVisibility(bIsFirstPerson, true);

#pragma region Weapon Default Setting

	//Weapon Factory 기능
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

	//WeaponFactory에서 생성한 무기를 장착한다.
	EquippedWeapons.Add(CurrentWeaponActor);
	EquippedWeapons.Add(SecondWeaponActor);
	EquippedWeapons.Add(ThirdWeaponActor);

	//시작할때 0번 슬롯 무기로 시작(1번키)
	ChangeWeapon(0);

	//시작할떄 유탄총은 안보이게 처리
	gunMeshComp->SetVisibility(false);

#pragma endregion

}

void ATPSPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(LaunchUpwardTimeHandle);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(ResetFrictionTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(SniperUITimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(LoadingHandle);
	GetWorld()->GetTimerManager().ClearTimer(DamagedHandle);
	GetWorld()->GetTimerManager().ClearTimer(GrenadeHandle);
	GetWorld()->GetTimerManager().ClearTimer(DashEffectDeactivateHandle);
	GetWorld()->GetTimerManager().ClearTimer(DetachTimerHandle);
	
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//PlayerHUD UI업데이트
	UpdateMyHUD();

	//카메라 위치를 머리 위치로 지속 갱신
	FVector newCameraLocation = GetMesh()->GetSocketLocation(TEXT("headSocket"));
	fpsCamComp->SetWorldLocation(newCameraLocation);

	UpdateAimingEnemy();
}

void ATPSPlayer::UpdateAimingEnemy()
{
	// 카메라를 가져옴
	UCameraComponent* CameraComponent = bIsFirstPerson ? fpsCamComp : tpsCamComp;
	if (CameraComponent == nullptr)
	{
		return;
	}

	// 라인트레이스 시작 위치와 방향 설정
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = Start + (CameraComponent->GetForwardVector() * 5000);

	// 라인트레이스 결과 저장을 위한 배열
	FHitResult HitResult;

	// 라인트레이스 매개변수 설정
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// 라인트레이스 수행
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		QueryParams
	);

	// 디버그용 라인트레이스 시각화
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 1.0f);
	if (bHit)
	{
		auto* actor = HitResult.GetActor();
		auto* enemy = Cast<AEnemy>(actor);
		if (enemy)
		{
			if (DetectedEnemy != enemy)
			{
				if (DetectedEnemy)
				{
					DetectedEnemy->AimEnemy(false);
				}

				// 현재 감지한 적을 저장하고 표시
				DetectedEnemy = enemy;
				DetectedEnemy->AimEnemy(true);
			}
		}
		else
		{
			if (DetectedEnemy)
			{
				DetectedEnemy->AimEnemy(false);
			}

			DetectedEnemy = nullptr;
		}
	}
	else
	{
		if (DetectedEnemy)
		{
			DetectedEnemy->AimEnemy(false);
		}

		DetectedEnemy = nullptr;
	}
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
	}
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
		
		AmmoText->SetText(FText::AsNumber(CurrentAmmo));
	}
	if (MaxAmmoText)
	{
		//MaxAmmoText의 값을 현재 장착한 무기의 Max 탄약수로 바인딩
		int32 MaxAmmo = CurrentWeaponActor->MaxWeaponAmmo;
		
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

//무기습득 이벤트처리함수
void ATPSPlayer::GainWeapon()
{
	if (nullptr != OverlappedWeapon && ! IsDetaching)
	{
		// 기존에 무기가 있다면 버린다.
		if (!CurrentWeaponActor->IsA<AMyEmptyWeaponActor>())
		{
			CurrentWeaponActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			//Detach할때 충돌처리가 바로 설정되면 문제가 자꾸 생겨서 지연처리하고 함수호출로 관리.
			GetWorldTimerManager().SetTimer(DetachTimerHandle, this, &ATPSPlayer::DetachWeapon, 0.1f, false);
			DetachWeaponActor = CurrentWeaponActor;
			IsDetaching = true;
			UE_LOG(LogTemp, Warning, TEXT("CurrentWeaponActor Detach!"));
		}
		else
		{
			CurrentWeaponActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CurrentWeaponActor->Destroy();
		}

		UE_LOG(LogTemp, Warning, TEXT("OverlappedWeapon Equipped!"));
		//들어온 무기정보로 현재 무기슬롯의 무기를 채운다.
		EquippedWeapons[CurrentWeaponIndex] = OverlappedWeapon;
		CurrentWeaponActor = OverlappedWeapon;
		
		//무기를 Attach해준다.
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeaponActor->AttachToComponent(FPSMesh, Rules, TEXT("GripPoint"));
		CurrentWeaponActor->WeaponBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CurrentWeaponActor->WeaponSpawnEffect->SetActive(false);
		CurrentWeaponActor->SetCamera(fpsCamComp);
		CurrentWeaponActor->SetPlayerBuff(AttackPower);

		OverlappedWeapon = nullptr;

		//Attach소리를 플레이한다.
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponAttachSound);
	}
}
//무기드랍 이벤트처리함수
void ATPSPlayer::DetachWeapon()
{
	if (DetachWeaponActor)
	{
		IsDetaching = false;
		DetachWeaponActor->WeaponBoxComp->SetSimulatePhysics(true);
		DetachWeaponActor->WeaponBoxComp->AddForce(FVector(0, 0, 10000));
		DetachWeaponActor->WeaponSpawnEffect->SetActive(true);
		DetachWeaponActor->WeaponBoxComp->SetCollisionProfileName(TEXT("Weapon"));
		DetachWeaponActor = nullptr;
	}
}

//무기오버랩 이벤트처리함수
void ATPSPlayer::OnMyOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

#pragma region Dash Event (LaunchCharacter)

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

		//대쉬소리를 플레이한다.
		UGameplayStatics::PlaySound2D(GetWorld(), PlayerDashSound);

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

#pragma region Weapon(Direct Control) Settings

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
	CurrentWeaponActor->WeaponStopAttack();
}

#pragma endregion
