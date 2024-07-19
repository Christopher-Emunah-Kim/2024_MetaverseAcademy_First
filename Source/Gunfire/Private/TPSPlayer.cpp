// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include "EnhancedInputSubsystems.h" //�߰�
#include "EnhancedInputComponent.h" //�߰�
#include "InputActionValue.h"  //�߰�
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Blueprint/UserWidget.h>
#include "Bullet.h"
#include "Grenade.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Components/ProgressBar.h> 
#include <Components/TextBlock.h> 
#include <Component/MyPlayerMove.h> //�߰�
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

	//TPSĳ���� ���̷�Ż�޽� �����͸� �ҷ�����
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Animation_Starter/UE4_Mannequin/Mesh/SK_Mannequin.SK_Mannequin'"));

	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);

		//2. Mesh ������Ʈ ��ġ�� ȸ������ ����
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

#pragma endregion

#pragma region Camera Settings

	//�⺻ ī�޶� ����
	//SpringArm ������Ʈ ���̱�
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	//���丮�Լ��� �ν��Ͻ� �����Ͽ� SpringArmComp������ ����
	springArmComp->SetupAttachment(RootComponent);
	//Root������Ʈ �ڽ����� ����
	springArmComp->SetRelativeLocation(FVector(0, 70, 90));
	//�θ� ������Ʈ �������� �����ǥ ����
	springArmComp->TargetArmLength = 400;
	//Ÿ����� �����Ÿ� ����
	springArmComp->bUsePawnControlRotation = true;
	//ȸ��ó�� ������ ���

	//TPS ī�޶� ���̰� �ʹ�
	//Camera ������Ʈ ���̱�
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	//���丮�Լ��� �ν��Ͻ� �����Ͽ� TpsCamComp������ ����
	tpsCamComp->SetupAttachment(springArmComp);
	//springCamComp�� �ڽ����� ���
	tpsCamComp->bUsePawnControlRotation = false;
	//ȸ��ó�� ������ ���

	//FPS ī�޶� ���̰� �ʹ�. 
	//Camera ������Ʈ ���̱�
	fpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FpsCamComp"));
	//���丮�Լ��� �ν��Ͻ� �����Ͽ� fpsCamComp������ ����
	fpsCamComp->SetupAttachment(GetMesh(), TEXT("headSocket"));
	//ī�޶� �÷��̾� �޽ÿ� ����
	fpsCamComp->SetRelativeLocation(FVector(0, 20, 0));
	fpsCamComp->SetRelativeRotation(FRotator(0, 90, -90));
	//�θ�������Ʈ �������� ��ġ/ȸ�� ����
	fpsCamComp->bUsePawnControlRotation = true;
	//ȸ��ó�� ������ ���
	fpsCamComp->SetActive(false);

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	//Ŭ���� ȸ��ó�� ������ ���
	//���� ī�޶� ���ư��� ĳ���Ͱ� �Բ� ���ư��� �ƴ����� ���� ����

	//1��Ī/3��Ī���� ��ȯ �⺻�� ����
	bIsFirstPerson = true;

#pragma endregion

#pragma region FPS Character Data Load
//fpscamcomp�� ���̴°Ŷ� ��ġ �ű�� �ȵ�.

	//FPSĳ���� �޽� ������Ʈ �ʱ�ȭ
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSMesh"));
	FPSMesh->SetupAttachment(fpsCamComp);
	FPSMesh->SetRelativeLocation(FVector(50.0f, 0.0f, -170.0f));
	FPSMesh->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	FPSMesh->SetOnlyOwnerSee(true); // FPS �޽ø� �÷��̾ �� �� �ֵ��� ����
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	//FPS�޽� ���̷�Ż �޽� �����ͷε�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> FPSMeshObject(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/FirstPersonArms/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms'"));
	if (FPSMeshObject.Succeeded())
	{
		FPSMesh->SetSkeletalMesh(FPSMeshObject.Object);
	}

#pragma endregion

#pragma region Weapon Data Load

	//1. ��ź�� ���̷�Ż�޽� ������Ʈ ���
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	//1-1. �θ� ������Ʈ�� Mesh������Ʈ�� ����
	gunMeshComp->SetupAttachment(FPSMesh, TEXT("GripPoint"));
	//1-2. ��ź�ѱ� ���̷�Ż�޽� ������ �ε�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/FPWeapon/Mesh/SK_FPGun.SK_FPGun'"));
	//1-3. �����ͷε尡 �����ߴٸ�
	if (TempGunMesh.Succeeded())
	{
		//1-4. ���̷�Ż �޽� ������ �Ҵ�
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
	}

	////2. �������۰� ������Ʈ ���
	//sniperGunComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SniperGunComp"));
	////2-1. �θ� ������Ʈ�� Mesh ������Ʈ�� ����
	//sniperGunComp->SetupAttachment(FPSMesh, TEXT("GripPoint"));
	////2-2. ���̷�Ż�޽� ������ �ε�
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> TempSniperMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MarketplaceBlockout/Modern/Weapons/Assets/Snipers/02/SKM_Modern_Weapons_Sniper_02.SKM_Modern_Weapons_Sniper_02'"));
	////2-3. �����ͷε尡 �����ߴٸ�
	//if (TempSniperMesh.Succeeded())
	//{
	//	//2-4. �ε��� ����ƽ�޽� ������ �Ҵ�
	//	sniperGunComp->SetSkeletalMesh(TempSniperMesh.Object);
	//}

	//�������۸�忡�� Ÿ�Ӷ��� ����
	//2-7. Ÿ�Ӷ��� ������Ʈ �ʱ�ȭ
	ZoomTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("ZoomTimeLine"));
	//2-8. ���� ��ġ �� ȸ�� �ʱ�ȭ
	OriginalGunTransform = FTransform::Identity;
	//2-9 ���ϴ� ��ǥ ��ġ �� ȸ������ ����
	TargetGunTransform = FTransform(FRotator(7.0f, 5.0f, 3.0f), FVector(15.0f, -70.0f, 23.0f)); 
     

	////3. SMG ������Ʈ ���
	//SMGMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SMGComp"));
	////3-1. �θ�������Ʈ�� ����
	//SMGMesh->SetupAttachment(FPSMesh, TEXT("GripPoint"));
	////3-2. ���̷�Ż�޽� �����ͷε�
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> TempSMGMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MarketplaceBlockout/Modern/Weapons/Assets/SMGs/01/SKM_Modern_Weapons_SMG_01.SKM_Modern_Weapons_SMG_01'"));
	////3-3. �����ͷε尡 �����ߴٸ�
	//if (TempSMGMesh.Succeeded())
	//{
	//	//3-4. ���̷�Ż�޽� ������ �Ҵ�
	//	SMGMesh->SetSkeletalMesh(TempSMGMesh.Object);
	//}

	////SMG�⺻����
	//SMGFireRate = 0.1f;  //1�ʿ� 10��
	////SMGDamage = 10.0f;
	//SMGRange = 4000.0f;

	////4. ShotGun ������Ʈ ���
	//ShotGunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ShotGunComp"));
	////4-1. �θ�������Ʈ�� ����
	//ShotGunMesh->SetupAttachment(FPSMesh, TEXT("GripPoint"));
	////4-2. ���̷�Ż�޽� �����ͷε�
	//ConstructorHelpers::FObjectFinder<USkeletalMesh> TempShotGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MarketplaceBlockout/Modern/Weapons/Assets/Shotguns/02/SKM_Modern_Weapons_Shotgun_02.SKM_Modern_Weapons_Shotgun_02'"));
	////4-3. �����ͷε尡 �����ߴٸ�
	//if (TempShotGunMesh.Succeeded())
	//{
	//	//4-4. ���̷�Ż�޽� ������ �Ҵ�
	//	ShotGunMesh->SetSkeletalMesh(TempShotGunMesh.Object);
	//}
	//
	////ShotGun �⺻ ����
	//ShotGunRange = 2000.0f;

#pragma endregion
	
#pragma region Movement Settings

	//2�� ���� ����
	JumpMaxCount = 2;
	//�ʱ� ���¸� ��� �������� ����
	bCanLaunch = true;
	//�⺻�����»��� ���� �ʱ�ȭ
	OriginalGroundFriction = GetCharacterMovement()->GroundFriction;
	//�޸��� ���� �ʱ�ȭ
	//bisRunning = false;

	// DashEffectScene ������Ʈ ����
	DashEffectScene = CreateDefaultSubobject<USceneComponent>(TEXT("DashEffectScene"));
	DashEffectScene->SetupAttachment(RootComponent); // RootComponent�� ����
	DashEffectScene->SetRelativeLocation(FVector(200, 0, 90)); // ���ϴ� ��ġ ����

	//�뽬 ����Ʈ ���̾ư��� ������Ʈ ���� 
	PlayerDashEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PlayerDashEffect"));
	PlayerDashEffect->SetupAttachment(DashEffectScene);
	PlayerDashEffect->bAutoActivate = false; // �⺻������ ��Ȱ��ȭ
	
#pragma endregion

#pragma region Player/Weapon Base Setting

	//�÷��̾��� �⺻ ���ݷ� ����
	AttackPower = 1.0f;
	//�÷��̾��� �⺻ HP����
	PlayerMAXHP = 300.0f;
	PlayerHP = PlayerMAXHP;

	//�÷��̾��� ATK���ȿ� ������ �޴� ���⺰ �⺻������ ����
	BaseGrenadeGunDamage = 50.0f;
	/*BaseSniperGunDamage = 100.0f;
	BaseSMGDamage = 15.0f;
	BaseShotGunDamage = 15.0f;*/
	BaseGrenadeDamage = 150.0f;

	//����ź �⺻����
	ThrowingForce = 800.0f;

#pragma endregion

}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

	// ���� ����۽� ���� �Ͻ����� Ǯ��
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	// �Է� ��� ���� �ʱ�ȭ
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;
	}

	//Enhanced Input Context ���
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

	////�ʱ� �ӵ��� �ȱ�� ����
	//GetCharacterMovement()->MaxWalkSpeed = runSpeed;

	//1-1. �������� UI ���� �ν��Ͻ� ����
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	//1-2. Ÿ�Ӷ��� ��� �����Ǿ��ִ��� Ȯ��
	if (ZoomCurve)
	{
		//1-3. Ÿ�Ӷ��ο� � �߰� �� ������Ʈ �Լ� ���ε�
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("ZoomTimelineProgress"));
		ZoomTimeLine->AddInterpFloat(ZoomCurve, ProgressFunction);
	}
	//1-4. ������ ������ġ�� ȸ�� ����
	OriginalGunTransform = gunMeshComp->GetRelativeTransform();

	//�Ϲ����� UI ����
	_crosshairUI = CreateWidget(GetWorld(), crosshairUIFactory);
	//PlayerHUD UI ����
	_playerHUDUI = CreateWidget(GetWorld(), playerHUDFactory);
	//������ UI ����
	_takedamageUI = CreateWidget(GetWorld(), TDamageUIFactory);
	//RestartUI ���� 
	_restartUI = CreateWidget(GetWorld(), RestartUIFactory);
	//LoadingUI ���� �� ȭ�鿡 �߰�
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

	//�ʱ� �÷��̾� ���� ����
	tpsCamComp->SetActive(!bIsFirstPerson);
	fpsCamComp->SetActive(bIsFirstPerson);

	//�ʱ� ���� ������ ���� �޽� ���ü� ����
	GetMesh()->SetVisibility(!bIsFirstPerson, true);
	FPSMesh->SetVisibility(bIsFirstPerson, true);

	////���⽽�� �ʱ�ȭ(�⺻���� SMG�� 1��)
	//WeaponSlots.Empty(3);
	//WeaponSlots.Add(EWeaponType::SMG);
	//WeaponSlots.Add(EWeaponType::SniperGun);
	//WeaponSlots.Add(EWeaponType::ShotGun);

	//�⺻���� SMG�� ����ϵ��� ����
	//ChangeWeapon(0);

	//// �ʱ� ź�� �� ����
	//CurrentSMGAmmo = MaxSMGAmmo;
	//CurrentSniperAmmo = MaxSniperAmmo;
	//CurrentShotGunAmmo = MaxShotGunAmmo;

	//������ �������Ŭ���� ��������
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


	//�������������� �����Ѵ�.
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

	//PlayerHUD UI������Ʈ
	UpdateMyHUD();

	//ī�޶� ��ġ�� �Ӹ� ��ġ�� ���� ����
	FVector newCameraLocation = GetMesh()->GetSocketLocation(TEXT("headSocket"));
	fpsCamComp->SetWorldLocation(newCameraLocation);

}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//���콺 ��Ʈ�� �̺�Ʈ ó���Լ� ���ε�
	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if(PlayerInput)
	{
		//������Ʈ���� �Է¹��ε��� ó���ϵ��� �����Լ� ȣ��
		playerMove->SetupInputBinding(PlayerInput);
		/*PlayerInput->BindAction(ia_Turn, ETriggerEvent::Triggered, this, &ATPSPlayer::Turn);
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::LookUp);*/
	}
	////�����¿��Է� �̺�Ʈó���Լ� ���ε�
	//PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::InputMove);
	////�����Է� �̺�Ʈó���Լ� ���ε�
	//PlayerInput->BindAction(ia_Jump, ETriggerEvent::Triggered, this, &ATPSPlayer::InputJump);
	//������ȯ �̺�Ʈó���Լ� ���ε�
	PlayerInputComponent->BindAction(TEXT("ToggleView"), IE_Pressed, this, &ATPSPlayer::ToggleView);
	//�Ѿ˹߻� �̺�Ʈó���Լ� ���ε�
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATPSPlayer::InputFire);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Released, this, &ATPSPlayer::StopFire);
	//�ѱⱳü �̺�Ʈó���Լ� ���ε�
	PlayerInputComponent->BindAction(TEXT("Weapon1"), IE_Pressed, this, &ATPSPlayer::Weapon1);
	PlayerInputComponent->BindAction(TEXT("Weapon2"), IE_Pressed, this, &ATPSPlayer::Weapon2);
	PlayerInputComponent->BindAction(TEXT("Weapon3"), IE_Pressed, this, &ATPSPlayer::Weapon3);
	//������ ���ⱳü�̺�Ʈ ó���Լ� ���ε�
	PlayerInputComponent->BindAction(TEXT("Function"), IE_Pressed, this, &ATPSPlayer::GainWeapon);
	//�������� ���ظ�� �̺�Ʈó���Լ� ���ε�
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Pressed, this, &ATPSPlayer::SniperAim);
	PlayerInputComponent->BindAction(TEXT("Sniper"), IE_Released, this, &ATPSPlayer::SniperAim);
	////�޸����Է� �̺�Ʈó���Լ� ���ε�
	//PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &ATPSPlayer::InputRun);
	////PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &ATPSPlayer::InputRun);
	//��ñ�� �̺�Ʈó���Լ� ���ε�
	PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &ATPSPlayer::InputLaunch);
	//����ź�߻� �̺�Ʈó���Լ� ���ε�
	PlayerInputComponent->BindAction(TEXT("Grenade"), IE_Pressed, this, &ATPSPlayer::InputGrenade);
	//������ �̺�Ʈó���Լ� ���ε�
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &ATPSPlayer::Reload);
}

//�ε��� UI �ϰ� AddtoViewport �Լ�
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

//Player HUD UI ������Ʈ �Լ� �߰�
void ATPSPlayer::UpdateMyHUD()
{
	if (!_playerHUDUI) return;

	//HUD�� HP Bar�� AMMO Text��Ͽ� ������ ���� ����
	UProgressBar* HPBar = Cast<UProgressBar>(_playerHUDUI->GetWidgetFromName(TEXT("HPBar")));
	UTextBlock* AmmoText = Cast<UTextBlock>(_playerHUDUI->GetWidgetFromName(TEXT("AmmoText")));
	UTextBlock* MaxAmmoText = Cast<UTextBlock>(_playerHUDUI->GetWidgetFromName(TEXT("MaxAmmoText")));

	//HPText�� MaxHPText �ؽ�Ʈ ��Ͽ� ������ ���� ����
	UTextBlock* HPText = Cast<UTextBlock>(_playerHUDUI->GetWidgetFromName(TEXT("HPText")));
	UTextBlock* MaxHPText = Cast<UTextBlock>(_playerHUDUI->GetWidgetFromName(TEXT("MaxHPText")));

	if (HPBar)
	{
		//���� ProgressBar�� �ۼ�Ʈ�� PlayerHP/100���� ���ε�
		HPBar->SetPercent( PlayerHP / PlayerMAXHP );
	}
	if (AmmoText)
	{
		//AmmoText�� ���� ���� ������ ������ ź����� ���ε�
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
		//MaxAmmoText�� ���� ���� ������ ������ Max ź����� ���ε�
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

	//PlayerHP�� PlayerMAXHP���� HPText�� MaxHPText�� ���� ����
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
		// ������ ���Ⱑ �ִٸ� ������.
		if (!CurrentWeaponActor->IsA<AMyEmptyWeaponActor>())
		{
			CurrentWeaponActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			CurrentWeaponActor->WeaponBoxComp->SetSimulatePhysics(true);
			CurrentWeaponActor->WeaponBoxComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
		}
		//���� ���������� ���� ���⽽���� ���⸦ ä���.
		EquippedWeapons[CurrentWeaponIndex] = OverlappedWeapon;
		CurrentWeaponActor = OverlappedWeapon;
		
		//���⸦ Attach���ش�.
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeaponActor->AttachToComponent(FPSMesh, Rules, TEXT("GripPoint"));
		CurrentWeaponActor->WeaponBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CurrentWeaponActor->SetCamera(fpsCamComp);
		CurrentWeaponActor->SetPlayerBuff(AttackPower);

		OverlappedWeapon = nullptr;
	}
}

//������� �̺�Ʈó���Լ�
void ATPSPlayer::OnMyOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapped!"));
	//�������� ���Ͱ� �����ϰ�, �� ���Ͱ� �� �ڽ��� �ƴϸ鼭, �������� ������Ʈ�� �����ϴ��� üũ
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

//1��Ī/3��Ī ���� ��ȯ�Լ� ����
void ATPSPlayer::ToggleView()
{
	//������ ī�޶�������� üũ ������ ����Ѵ�.
	bIsFirstPerson = !bIsFirstPerson;

	//�� ����üũ ��Ȳ�� ���� TPS/FPSī�޶� Ȱ��ȭ�Ѵ�.
	tpsCamComp->SetActive(!bIsFirstPerson);
	fpsCamComp->SetActive(bIsFirstPerson);

	//���� FPS/TPS ī�޶� ���¿� ���� ĳ���� �޽ø� Ȱ��ȭ�Ѵ�.
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

//�ǰ� ������ �̺�Ʈó���Լ� ����
float ATPSPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// ������ UI�� ȭ�鿡 ǥ��
	if (nullptr != _takedamageUI)
	{
		//_takedamageUI->SetVisibility(ESlateVisibility::Visible);
		_takedamageUI->AddToViewport(1);
	}

	//�÷��̾� HP ����ó��
	PlayerHP -= FinalDamage;
	if (PlayerHP <= 0)
	{
		PlayerHP = 0;

		// ���� ���߱�
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		//�÷��̾ ��������� UI���
		_sniperUI->SetVisibility(ESlateVisibility::Hidden);
		_crosshairUI->SetVisibility(ESlateVisibility::Hidden);
		_playerHUDUI->SetVisibility(ESlateVisibility::Hidden);

		// _restartUI�� ȭ�鿡 �߰�
		if (_restartUI)
		{
			_restartUI->SetVisibility(ESlateVisibility::Visible);
			//_restartUI->AddToViewport();

			// �Է� ��� ����
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				// ���콺 Ŀ�� ���̰� �ϱ�
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

	// ������ �ε������� UI���� HitLocation���� ����
	if (DamageCauser)
		DamageIndicator(DamageCauser->GetActorLocation());


	// ���� �ð� �� ������ UI�� ����
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

////���콺 ��Ʈ�� �̺�Ʈó���Լ� ����
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

////�����¿��̵� �Է��̺�Ʈ ó���Լ�
//void ATPSPlayer::InputMove(const struct FInputActionValue& inputValue)
//{
//	FVector2D value = inputValue.Get<FVector2D>();
//	//�����Է��̺�Ʈó��
//	direction.X = value.X;
//	//�¿��Է��̺�Ʈó��
//	direction.Y = value.Y;
//}

////Jump �̺�Ʈó���Լ� ����
//void ATPSPlayer::InputJump(const struct FInputActionValue& inputValue)
//{
//	Jump();
//}

////�̵��Լ� Move(����ȭ)
//void ATPSPlayer::MyMove()
//{
//	//������ǥ�� �����ǥ�� ����
//	direction = FTransform(GetControlRotation()).TransformVector(direction);
//	//�𸮾󿡼� �����ϴ� �⺻ �����Ʈ ������Ʈ ���
//	AddMovementInput(direction);
//	direction = FVector::ZeroVector; //�ʱ�ȭ
//}

////�޸��� �̺�Ʈó�� �Լ� ����
//void ATPSPlayer::InputRun()
//{
//	//�޸��� ���¸� ��� ������� ����
//	bisRunning = !bisRunning;
//
//	//�޸�����¿� ���� �ӵ�����
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

//�뽬 �̺�Ʈó�� �Լ� ����
void ATPSPlayer::InputLaunch()
{
	//���� ���� �뽬�� ������ ���¶��
	if (bCanLaunch)
	{
		//1. cooldown�� ���������� ������ false�� ����
		bCanLaunch = false;
		//2. �뽬�ϴ� ���� ������ 0�� ���·� ����
		GetCharacterMovement()->GroundFriction = 0.0f;
		//3. ���ϴ� �������� �̵��ϰ�
		LaunchVelocity();
		//4. 0.01 ������ �� UpVector �̵�
		GetWorldTimerManager().SetTimer(LaunchUpwardTimeHandle, this, &ATPSPlayer::LaunchUpward, 0.01f, false);
		//5. 1�� ������ �� Cooldown �ʱ�ȭ
		GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ATPSPlayer::ResetLaunchCooldown, 1.0f, false);
		//6. ��ٿ� �ʱ�ȭ �� ������ ����
		GetWorldTimerManager().SetTimer(ResetFrictionTimerHandle, this, &ATPSPlayer::ResetGroundFriction, 1.0f, false);

		// ��� �� ���Ʒ� ��鸲 ����
		//ApplyMeshBobbing(GetWorld()->GetDeltaSeconds(), 4.0f); // ��� �� ���� ��鸲 ����
		//GetWorldTimerManager().SetTimer(BobbingIntensityResetTimerHandle, [this]()
		//	{
		//		// 2.0�� �� ��鸲 ������ ���� ���·� �ǵ���
		//		ApplyMeshBobbing(GetWorld()->GetDeltaSeconds(), 1.0f);
		//	}, 2.0f, false);

		// ��� ���� �� ���̾ư��� ����Ʈ ����
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

//���ϴ� �������� �뽬 ��� �Լ� ����
void ATPSPlayer::LaunchVelocity()
{
	//������ �̵���ų Velocity
	//FVector LaunchVelocity = GetActorForwardVector() * 1000; //�����θ� �̵��� ���Ҷ�
	//���ϴ� �������� �̵���ų Veloicity ���� ����
	FVector LaunchVelocity = GetVelocity().GetSafeNormal() * 1400;
	//ĳ���͸� ������ �̵�
	LaunchCharacter(LaunchVelocity, true, false);

	// ���̾ư��� ����Ʈ ����
	if (PlayerDashEffect && PlayerDashEffect->GetAsset())
	{
		GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Red, FString::Printf(TEXT("DashEffect : %d"), PlayerDashEffect));
		PlayerDashEffect->Activate(true);
	}
}

//�뽬�ҋ� ���� ��¦ �߰��ϴ� �Լ� ����
void ATPSPlayer::LaunchUpward()
{
	//���� 300�� ������ �̵���ų ����
	FVector LaunchVelocity = FVector(0, 0, 150);
	LaunchCharacter(LaunchVelocity, false, true);
}

//Cooldown �ʱ�ȭ �Լ� ����
void ATPSPlayer::ResetLaunchCooldown()
{
	bCanLaunch = true;
}

//�����»��� �ʱ�ȭ �Լ� ����
void ATPSPlayer::ResetGroundFriction()
{
	GetCharacterMovement()->GroundFriction = OriginalGroundFriction;
}

#pragma endregion

#pragma region Firing Setting

//�Ѿ˹߻�ó���Լ�
void ATPSPlayer::InputFire()
{
	//�߻� �ִϸ��̼� ó��
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
	//bIsFiring = true;//�߻� ���� üũ

	//Switch���� �̿��Ͽ� �ѱ�߻�ó��
	//���� ����Ÿ�Կ� ���� ź��� Ȯ�� �� �߻�ó��
	//switch (CurrentWeaponType)
	//{
	//case EWeaponType::GrenadeGun:
	//{
	//	// �ѱ� ȭ�� ��ƼŬ ����
	//	if (MuzzleFlash)
	//	{
	//		FTransform MuzzleTransform = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
	//	}

	//	//�� ���̷�Ż�޽� ������Ʈ�� �ѱ���ġ ������ �����´�.
	//	FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
	//	//�ѱ���ġ�� �Ѿ˰����� ��ġ�ϰ�, �ű⿡�� �Ѿ��� �����Ѵ�.
	//	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	//	if (Bullet)
	//	{
	//		Bullet->Damage = BaseGrenadeGunDamage * AttackPower;
	//	}

	//	//�߻���� ����
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

//����źâ ������ �Լ�
void ATPSPlayer::Reload()
{
	
	//������ �ִϸ��̼� ó��
	auto* anim = FPSMesh->GetAnimInstance();
	if (anim)
	{
		anim->Montage_Play(ReloadMontage);
	}
	CurrentWeaponActor->WeaponReload();
   
	// (�������ִϸ��̼ǽð�)1.0�� �Ŀ� ź���� ä��� �߻� ���� ���·� ����
	//GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, [this]()
	//	{
	//		// ���� ���� Ÿ�Կ� ���� ������ ó��
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

//���⺯�� ���� �Լ�
void ATPSPlayer::ChangeWeapon(int32 slot)
{
	
	//���� �����ε����� slot���� ����
	CurrentWeaponIndex = slot;
	//����ڵ�
	if (nullptr != CurrentWeaponActor)
	{
		//���� ������ ���ü� ����
		CurrentWeaponActor->GunMesh->SetVisibility(false);
	}
	//���� ���⸦ slot�� �ش��ϴ� ����� ����
	CurrentWeaponActor = EquippedWeapons[slot];
	//����ڵ�
	if (nullptr != CurrentWeaponActor)
	{
		//���� ������ ���ü� ����
		CurrentWeaponActor->GunMesh->SetVisibility(true);
	}

	////���޵� ���� �ε����� ���� ������ �������� ������ üũ(false�� ���۾��ϰ� �Լ� ����)
	//if (slot < WeaponSlots.Num())
	//{
	//	//�־��� �����ε����� ���⸦ ���繫��Ÿ������ ����
	//	CurrentWeaponType = WeaponSlots[slot];
	//	//��������UI��뿩�� üũ�� ���� ���繫��Ÿ���� ������Ÿ������ üũ 
	//	bUsingSniperGun = CurrentWeaponType == EWeaponType::SniperGun;

		//FPS����� ��� �ѱ� �޽ø� FPS���� �޽ÿ� ����(�̻��)
		//if (bIsFirstPerson)
		//{
		//	gunMeshComp->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	sniperGunComp->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	SMGMesh->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	ShotGunMesh->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		//}
		////TPS����� ��� �ѱ� �޽ø� TPS�޽ÿ� ����(�̻��)
		//else
		//{
		//	gunMeshComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	sniperGunComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	SMGMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		//	ShotGunMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);

		//}

	//	//����޽� ���ü� ����
	//	gunMeshComp->SetVisibility(CurrentWeaponType == EWeaponType::GrenadeGun);
	//	sniperGunComp->SetVisibility(CurrentWeaponType == EWeaponType::SniperGun);
	//	SMGMesh->SetVisibility(CurrentWeaponType == EWeaponType::SMG);
	//	ShotGunMesh->SetVisibility(CurrentWeaponType == EWeaponType::ShotGun);
	//}
}

//1�� ���� ���� �Լ�
void ATPSPlayer::Weapon1()
{
	ChangeWeapon(0);
}
//2�� ���� ���� �Լ�
void ATPSPlayer::Weapon2()
{
	ChangeWeapon(1);
}
//3�� ���� ���� �Լ�
void ATPSPlayer::Weapon3()
{
	ChangeWeapon(2);
}


#pragma endregion

#pragma region Weapon Settings

//�������� �߻�ó���Լ�(LineTrace Ȱ��)
//void ATPSPlayer::HandleSniperFire()
//{
//	// ī�޶� ������Ʈ ����
//	UCameraComponent* activeCamComp = bIsFirstPerson ? fpsCamComp : tpsCamComp;
//
//	//LineTrace�� ������ġ(Ȱ��ȭ�� ī�޶� ����)
//	FVector startPos = activeCamComp->GetComponentLocation();
//	//LineTrace�� ������ġ(Ȱ��ȭ�� ī�޶� ���߾ӿ��� �������� N��ŭ)
//	FVector endPos = startPos + (activeCamComp->GetForwardVector() * 15000);
//	//LineTrace�� �浹������ ���� ����
//	FHitResult hitInfo;
//	//�浹�ɼ� ��������
//	FCollisionQueryParams params;
//	//�ڱ��ڽ�(�÷��̾�)�� �浹���� ����
//	params.AddIgnoredActor(this);
//	//Channel ���͸� �̿��� LineTrace �浹 ����
//	//(�浹����, ������ġ, ������ġ, ����ä��, �浹�ɼ�)
//	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
//	
//	// ���̾ư��� �ý��� ����
//	UNiagaraComponent* TrailNSC = nullptr;
//	if (SniperTrail)
//	{
//		FVector TraceVector = hitInfo.ImpactPoint - startPos;
//		FVector SpawnPos = startPos + (TraceVector * 0.1f); // 10% �������� ����
//		FRotator TrailRotation = TraceVector.Rotation();
//		TrailNSC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SniperTrail, SpawnPos, TrailRotation);
//	}
//
//	//LineTrace�� �ε�������(bHit�� true�� ��)
//	if (bHit)
//	{
//		//�浹ó�� -> �Ѿ� ����ȿ�� ���
//		//�Ѿ�����ȿ�� Ʈ������ ���� ����
//		FTransform bulletTrans;
//		//�ε�ģ ��ġ �Ҵ�
//		bulletTrans.SetLocation(hitInfo.ImpactPoint);
//		//�Ѿ� ���� ȿ�� �ν��Ͻ� ����
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);
//
//		//�ǰݵ� ���Ϳ� ������ ����
//		UGameplayStatics::ApplyPointDamage(hitInfo.GetActor(), BaseSniperGunDamage * AttackPower, activeCamComp->GetForwardVector(), hitInfo, GetController(), this, nullptr);
//
//		//�ǰݹ�ü ���������� ����
//		auto hitComp = hitInfo.GetComponent();
//		//1. ���� ���� ������Ʈ�� ������ ����Ǿ��ִٸ�
//		if (hitComp && hitComp->IsSimulatingPhysics())
//		{
//			//2. �������� ���� ������ ������ �����ְ�
//			FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 50000;
//			//3. �� �������� ����������.
//			hitComp->AddForce(force);
//		}
//
//		// bHit�� ������ 0.4�� ������ ���̾ư��� �ý��� ��Ȱ��ȭ
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
//	// �ѱ� ȭ�� ��ƼŬ ����
//	if (MuzzleFlash)
//	{
//		FTransform MuzzleTransform = sniperGunComp->GetSocketTransform(TEXT("FirePosition"));
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
//	}
//
//	//�߻���� ����
//	bIsFiring = true;
//}

//�������� ���� �Լ� ����
void ATPSPlayer::SniperAim()
{
	// �������۰� ��尡 �ƴ϶�� ó������ �ʴ´�.
	/*if (bUsingSniperGun == false)
	{
		return;
	}*/
	if (CurrentWeaponActor->bSniperMode == false)
	{
		return ;
	}

	// Pressed �Է� ó��
	if (!bSniperAim)
	{
		// 1. �������� ���� ��� Ȱ��ȭ
		bSniperAim = true;

		if (bIsFirstPerson == true)
		{
			// 2. 0.28�� �Ŀ� �������� ���� UI Ȱ��ȭ (1��Ī ���)
			GetWorldTimerManager().SetTimer(SniperUITimerHandle, FTimerDelegate::CreateLambda([this]()
				{
					if (bSniperAim)
					{
						_sniperUI->AddToViewport(0);
						//_crosshairUI->RemoveFromParent();
						//_sniperUI->SetVisibility(ESlateVisibility::Visible);
						_crosshairUI->SetVisibility(ESlateVisibility::Hidden);
						fpsCamComp->SetFieldOfView(45.0f);
						//�������� �� �޽� ���ü� ����
						CurrentWeaponActor->GunMesh->SetVisibility(false);
					}
				}), 0.28f, false);

			// Ÿ�Ӷ��� �� ��� ����
			StartZoom();
		}
		else
		{
			// 2. �������� ���� UI ��� (3��Ī ���)
			//_sniperUI->AddToViewport(0);
			//_crosshairUI->RemoveFromParent();
			_sniperUI->SetVisibility(ESlateVisibility::Visible);
			_crosshairUI->SetVisibility(ESlateVisibility::Hidden);
			// ī�޶��� �þ߰� Field Of View ����
			tpsCamComp->SetFieldOfView(45.0f);
			//�������� �� �޽� ���ü� ����
			CurrentWeaponActor->GunMesh->SetVisibility(false);
		}
	}
	// Released �Է� ó��
	else
	{
		// 1. �������� ���� ��� ��Ȱ��ȭ
		bSniperAim = false;

		// Ÿ�̸� ����
		GetWorldTimerManager().ClearTimer(SniperUITimerHandle);

		// 2. �������� ���� UI ȭ�鿡�� ����
		_sniperUI->RemoveFromParent();
		//_crosshairUI->AddToViewport(2);
		//_sniperUI->SetVisibility(ESlateVisibility::Hidden);
		_crosshairUI->SetVisibility(ESlateVisibility::Visible);

		// 3. ī�޶� �þ߰� ������� ����
		fpsCamComp->SetFieldOfView(90.0f);
		tpsCamComp->SetFieldOfView(90.0f);
		// 4. �Ϲ� ���� UI ���
		// 5. �������� �� �޽� ���ü� ����
		CurrentWeaponActor->GunMesh->SetVisibility(true);

		if (bIsFirstPerson)
		{
			// Ÿ�Ӷ��� �� ��� ����
			EndZoom();
		}
	}
}

//Ÿ�Ӷ��� �ܸ�� ����-�����Լ� ����
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

//�������� ���ظ�� Ÿ�Ӷ��� �Լ� ����
void ATPSPlayer::ZoomTimeLineProgress(float value)
{
	// �������۰� ��尡 �ƴϰų� 3��Ī ����� ó������ �ʴ´�.
	if (CurrentWeaponActor->bSniperMode == false || bIsFirstPerson == false)
	{
		return;
	}

	// ���� ��ġ�� ȸ�� ����
	FVector NewLocation = FMath::Lerp(OriginalGunTransform.GetLocation(), TargetGunTransform.GetLocation(), value);
	FRotator NewRotation = FMath::Lerp(OriginalGunTransform.GetRotation().Rotator(), TargetGunTransform.GetRotation().Rotator(), value);
	CurrentWeaponActor->GunMesh->SetRelativeLocationAndRotation(NewLocation, NewRotation);

	// ī�޶��� �þ߰� Field Of View ����
	tpsCamComp->SetFieldOfView(FMath::Lerp(90.0f, 45.0f, value));
}

//����ź�߻� �Լ� ����
void ATPSPlayer::InputGrenade()
{
	if (GrenadeFactory)
	{
		//1. ����ź�� ���� ��ġ ���(ĳ���� ��ġ���� �ణ �ڷ� ����)
		FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 50);
		//2. ĳ������ ȸ�������� ����
		FRotator SpawnRotation = GetControlRotation();
		//3. GrenadeFactory�� �̿��ؼ� ����ź ����
		AGrenade* Grenade = GetWorld()->SpawnActor<AGrenade>(GrenadeFactory, SpawnLocation, SpawnRotation);

		if (Grenade)
		{
			//4. ������ ������ ������ ���;��
			FVector LaunchDirection = GetActorForwardVector() * 0.7 + GetActorUpVector() * 0.5;
			//5. ����ź�� �������� �����ϱ� ���� UProjectileMovement�� Velocity�Ӽ��� ���
			//����ź�� �������� ����� �ӷ¿� ���� ����
			Grenade->GetProjectileMovementComponent()->Velocity = LaunchDirection * ThrowingForce;
			// ����ź ������ ����
			Grenade->Damage = BaseGrenadeDamage * AttackPower; // ����ź ������ ����
			Grenade->ExplosionRadius = 300.0f; // ����ź ���� �ݰ� ���� (�ʿ信 ���� ����)

			// ���� �ð� �� �����ϵ��� Ÿ�̸� ����
			GetWorldTimerManager().SetTimer(GrenadeHandle, FTimerDelegate::CreateLambda([Grenade]()
				{
					Grenade->Explode();
				}), 1.8f, false); // 1.8�� �� ����

		}
	}
}

//SMG �߻����� ó���Լ�
void ATPSPlayer::StopFire()
{
	////�߻���� üũ
	//bIsFiring = false;
	////SMG�߻�����ó��
	//GetWorldTimerManager().ClearTimer(SMGFIreTimer);
	CurrentWeaponActor->WeaponStopAttack();
}

////SMG �߻�ó���Լ�(LineTraceȰ��)
//void ATPSPlayer::HandleSMGFire()
//{
//	UE_LOG(LogTemp, Warning, TEXT("AMMO : %d"), CurrentSMGAmmo);
//	GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Blue, FString::Printf(TEXT("AMMO : %d"), CurrentSMGAmmo));
//
//	if (CurrentSMGAmmo > 0)
//	{
//		CurrentSMGAmmo = CurrentSMGAmmo - 1;
//	}
//	//ź����� 0�̸� �߻� ���� �� ������
//	if (CurrentSMGAmmo <= 0)
//	{
//		StopFire();
//		Reload();
//		return;
//	}
//
//	// ī�޶� ������Ʈ ����
//	UCameraComponent* activeCamComp = bIsFirstPerson ? fpsCamComp : tpsCamComp;
//
//	//LineTrace�� ������ġ(Ȱ��ȭ�� ī�޶� ��ġ)
//	FVector startPos = activeCamComp->GetComponentLocation();
//	//LineTrace�� ������ġ(Ȱ��ȭ�� ī�޶� ���߾ӿ��� �������� SMGRange��ŭ)
//	FVector endPos = startPos + (activeCamComp->GetForwardVector() * SMGRange);
//	//LineTrace�� �浹������ ���� ����
//	FHitResult hitInfo;
//	//�浹�ɼ� ��������
//	FCollisionQueryParams params;
//	//�ڱ��ڽ�(�÷��̾�)�� �浹���� ����
//	params.AddIgnoredActor(this);
//	//Channel ���͸� �̿��� LineTrace �浹 ����
//	//(�浹����, ������ġ, ������ġ, ����ä��, �浹�ɼ�)
//	FHitResult HitResult;
//	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
//
//	// ���̾ư��� �ý��� ����
//	UNiagaraComponent* TrailNSC = nullptr;
//	if (SMGTrail)
//	{
//		FVector TraceVector = hitInfo.ImpactPoint - startPos;
//		FVector SpawnPos = startPos + (TraceVector * 0.5f); // 50% �������� ����
//		FRotator TrailRotation = TraceVector.Rotation();
//		TrailNSC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SniperTrail, SpawnPos, TrailRotation);
//	}
//
//	//LineTrace�� �ε�������(bHit�� true�� ��)
//	if (bHit)
//	{
//		//�浹ó�� -> �Ѿ� ����ȿ�� ���
//		//�Ѿ�����ȿ�� Ʈ������ ���� ����
//		FTransform bulletTrans;
//		//�ε�ģ ��ġ �Ҵ�
//		bulletTrans.SetLocation(hitInfo.ImpactPoint);
//		//�Ѿ� ���� ȿ�� �ν��Ͻ� ����
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);
//
//		//������ ���Ϳ� ������ ����(�÷��̾� ATK �ݿ�)
//		UGameplayStatics::ApplyPointDamage(hitInfo.GetActor(), BaseSMGDamage * AttackPower, activeCamComp->GetForwardVector(), HitResult, GetController(), this, nullptr);
//
//		//�ǰݹ�ü ���������� ����
//		auto hitComp = hitInfo.GetComponent();
//		//1. ���� ���� ������Ʈ�� ������ ����Ǿ��ִٸ�
//		if (hitComp && hitComp->IsSimulatingPhysics())
//		{
//			//2. �������� ���� ������ ������ �����ְ�
//			FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 5000;
//			//3. �� �������� ����������.
//			hitComp->AddForce(force);
//		}
//
//		// bHit�� ������ 0.3�� ������ ���̾ư��� �ý��� ��Ȱ��ȭ
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
//	// �ѱ� ȭ�� ��ƼŬ ����
//	if (MuzzleFlash)
//	{
//		FTransform MuzzleTransform = SMGMesh->GetSocketTransform(TEXT("FirePosition"));
//		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
//	}
//
//	//�߻���� ����
//	bIsFiring = true;
//}

////ShotGun �߻�ó���Լ�(LineTrace Ȱ��)
//void ATPSPlayer::HandleShotGunFire()
//{
//	// ī�޶� ������Ʈ ����
//	UCameraComponent* activeCamComp = bIsFirstPerson ? fpsCamComp : tpsCamComp;
//
//	FVector startPos = activeCamComp->GetComponentLocation();
//	FVector forwardVector = activeCamComp->GetForwardVector();
//
//	//���������� ��������� ���� ����
//	float horizontalAngleStep = 4.0f; //���򰢵� ����
//	float verticalAngleStep = 4.0f;   //�������� ����
//	int32 numTraces = 8; //����Ʈ���̽� ����
//
//	//10���� ����Ʈ���̽����� ��������� ����(���Ǹ��)
//	for (int32 i = 0; i < numTraces; i++)
//	{
//		//������������ �����Ͽ� �������·� �������� ��
//		float randomYaw = FMath::RandRange(-horizontalAngleStep, horizontalAngleStep);
//		float randomPitch = FMath::RandRange(-verticalAngleStep, verticalAngleStep);
//
//		//������ ȸ�����ͷ� ��ȯ
//		FRotator randomRotator = FRotator(randomPitch, randomYaw, 0.0f);
//		FVector rotatedVector = randomRotator.RotateVector(forwardVector);
//		FVector endPos = startPos + (rotatedVector * ShotGunRange);
//		
//		//�浹���� ����
//		FHitResult hitInfo;
//		FCollisionQueryParams params;
//		//�� �ڽ��� �浹 ����
//		params.AddIgnoredActor(this);
//
//		//����Ʈ���̽����� �� ������ ���� �浹 �� ������ ����
//		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
//
//		// ���̾ư��� �ý��� ����
//		UNiagaraComponent* TrailNSC = nullptr;
//		if (ShotGunTrail)
//		{
//			FVector TraceVector = hitInfo.ImpactPoint - startPos;
//			FVector SpawnPos = startPos + (TraceVector * 0.7f); // 70% �������� ����
//			FRotator TrailRotation = TraceVector.Rotation();
//			TrailNSC = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SniperTrail, SpawnPos, TrailRotation);
//		}
//
//		if (bHit)
//		{
//			// �浹 ó�� -> �Ѿ� ���� ȿ�� ���
//			FTransform bulletTrans;
//			bulletTrans.SetLocation(hitInfo.ImpactPoint);
//			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);
//
//			// ������ ���Ϳ� ������ ����
//			UGameplayStatics::ApplyPointDamage(hitInfo.GetActor(), BaseShotGunDamage * AttackPower, rotatedVector, hitInfo, GetController(), this, nullptr);
//
//
//			// �ǰ� ��ü ���������� ����
//			auto hitComp = hitInfo.GetComponent();
//			if (hitComp && hitComp->IsSimulatingPhysics())
//			{
//				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 3000;
//				hitComp->AddForce(force);
//			}
//
//			// bHit�� ������ 0.2�� ������ ���̾ư��� �ý��� ��Ȱ��ȭ
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
//		//���� �ݵ� ����
//		/*ApplyRecoil(FVector(-30.0f, 0.0f, -30.0f), FRotator(-15.0f, 0.0f, 0.0f), 0.15f);*/
//		//�߻���� ����
//		bIsFiring = true;
//
//		// �ѱ� ȭ�� ��ƼŬ ����
//		if (MuzzleFlash)
//		{
//			FTransform MuzzleTransform = ShotGunMesh->GetSocketTransform(TEXT("FirePosition"));
//			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleTransform);
//		}
//
//		// ����� ���� �׸���
//		//DrawDebugLine(GetWorld(), startPos, endPos, bHit ? FColor::Green : FColor::Red, false, 1, 0, 1);
//	}
//}

#pragma endregion

#pragma region THE GRAVE

////�ִϸ��̼� �������� �ݵ�ȿ�� ���� ���� ����
//ī�޶� �ݵ� ������Ʈ
/*UpdateRecoil(DeltaTime);*/

//��� �⺻ ��鸲 ���� ����
//ApplyMeshBobbing(GetWorld()->GetDeltaSeconds(), 1.0f);

//��ź�߻�ݵ� ����
/*ApplyRecoil(FVector(-6.0f, 0.0f, -6.0f), FRotator(-3.0f, 0.0f, 0.0f), 0.1f);*/

//�������� �ݵ� ����
/*ApplyRecoil(FVector(-20.0f, 0.0f, -20.0f), FRotator(-8.0f, 0.0f, 0.0f), 0.2f);*/

//SMG �߻�ݵ� ����
/*ApplyRecoil(FVector(-20.0f, 0.0f, -20.0f), FRotator(-0.40f, 0.0f, 0.0f), 0.05f);*/

//�ѱ�ݵ�ó���Լ�(�ִϸ��̼� ��ü)
//void ATPSPlayer::ApplyRecoil(FVector NewRecoilOffset, FRotator NewRecoilRotation, float RecoilDuration)
//{
//	// �߻� ���� �� ī�޶��� Ʈ������ ���� ����
//	if (bIsFirstPerson)
//	{
//		InitialCameraTransform = fpsCamComp->GetComponentTransform();
//	}
//	else
//	{
//		InitialCameraTransform = tpsCamComp->GetComponentTransform();
//	}
//
//	//�ݵ��� ����
//	RecoilOffset = NewRecoilOffset;
//	RecoilRotation = NewRecoilRotation;
//	RecoilDuration = 0.1f;
//
//	//�����ð� �Ŀ� �ݵ��� �缳���ϵ��� Ÿ�̸� ����
//	GetWorldTimerManager().SetTimer(RecoilTimerHandle, this, &ATPSPlayer::ResetRecoil, RecoilDuration, false);
//}

//�ݵ� �ִϸ��̼� �缳�� �Լ�(�ִϸ��̼� ��ü)
//void ATPSPlayer::ResetRecoil()
//{
//	// �ݵ� �ʱ�ȭ
//	RecoilOffset = FVector::ZeroVector;
//	RecoilRotation = FRotator::ZeroRotator;
//
//	// �߻� ���� �ʱ�ȭ
//	bIsFiring = false;
//
//	// ���� �޽��� ��ġ�� ȸ�� �ʱ�ȭ
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
//		//0.2�ʸ��� ���� �޽���ġ�� �����ϵ��� ����
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
//	// ī�޶��� Ʈ������ ������ �ʱ� ������ ����
//	if (bIsFirstPerson)
//	{
//		fpsCamComp->SetWorldTransform(InitialCameraTransform);
//	}
//	else
//	{
//		//tpsCamComp->SetWorldTransform(InitialCameraTransform);
//	}
//}

//�ݵ� �ִϸ��̼� ������Ʈ �Լ�(�ִϸ��̼� ��ü)
//void ATPSPlayer::UpdateRecoil(float DeltaTime)
//{
//	if (bIsFiring)
//	{
//		//ī�޶� ��ġ�� ȸ���� �ݵ��� ����
//		if (bIsFirstPerson)
//		{
//			//FPSī�޶� �ݵ� ����
//			APlayerController* PlayerController = GetController<APlayerController>();
//			if (PlayerController)
//			{
//				// ��Ʈ�ѷ��� ���� ī�޶� �ݵ� ����
//				PlayerController->AddPitchInput(RecoilRotation.Pitch * 1.5f * DeltaTime);
//				PlayerController->AddYawInput(RecoilRotation.Yaw * 1.5f * DeltaTime);
//			}
//		}
//		//else
//		//{
//		//	// TPS ī�޶� �ݵ� ����
//		//	APlayerController* PlayerController = GetController<APlayerController>();
//		//	if (PlayerController)
//		//	{
//		//		// ��Ʈ�ѷ��� ���� ī�޶� �ݵ� ����
//		//		PlayerController->AddPitchInput(RecoilRotation.Pitch * DeltaTime);
//		//		PlayerController->AddYawInput(RecoilRotation.Yaw * DeltaTime);
//		//	}
//		//}
//		// ���� �޽��� ��ġ�� ȸ���� �ݵ� �� ����
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

//�ڵ��߻�Ǵ� SMG�� ��� ���� �ݵ��ʱ�ȭ(�ִϸ��̼� ��ü)
//void ATPSPlayer::ResetSMGPosition()
//{
//	SMGMesh->SetRelativeLocation(FVector(-14, 52, 130));
//	SMGMesh->SetRelativeRotation(FRotator::ZeroRotator);
//}

////�ִϸ��̼� �������� ī�޶���鸲 ȿ�� ���� ���� ����
//FPS�޽��� �ʱ� ��ġ ����(�޽���鸲 üũ�뵵)
/*InitialMeshLocation = FPSMesh->GetRelativeLocation();*/

/*GetWorld()->GetTimerManager().ClearTimer(RecoilTimerHandle);
GetWorld()->GetTimerManager().ClearTimer(SMGResetTimerHandle);*/

//�̵��� �޽���鸲 �Լ� ȣ��
/*ApplyMeshBobbing(DeltaTime, 1.0f);*/

//GetWorld()->GetTimerManager().ClearTimer(BobbingIntensityResetTimerHandle);

// �޽� ��鸲 �Լ� ����
//void ATPSPlayer::ApplyMeshBobbing(float DeltaTime, float BobbingIntensity)
//{
//	// �ѱ� �߻� �߿��� ��鸲 ������
//	if (bIsFiring)
//	{
//		//�ʱ� �޽� ��ġ�� ������
//		FPSMesh->SetRelativeLocation(InitialMeshLocation);
//		return;
//	}
//	// ��鸲 �ӵ��� ũ�� ����
//	float BobbingSpeed = 3.0f; // ��鸲 �ӵ�
//	float BobbingAmount = 0.08f * BobbingIntensity; // ��鸲 ũ��
//
//	// ���� �޽� ��ġ ��������
//	FVector NewLocation = FPSMesh->GetRelativeLocation();
//
//	// Z �� �������� �����ĸ� �̿��� ��鸲 ����
//	NewLocation.Z += FMath::Sin(GetWorld()->TimeSeconds * BobbingSpeed) * BobbingAmount;
//
//	// ���ο� ��ġ ����
//	FPSMesh->SetRelativeLocation(NewLocation);
//}

////���� ���⸦ �ٴڿ� ����ϴ� �Լ�
//void ATPSPlayer::DropCurrentWeapon()
//{
//	//���� ����ִ� ������¸� ���� ���� ����
//	USkeletalMeshComponent* CurrentWeaponMesh = nullptr;
//	//���� ���� ������ ���� ����� Mesh���� ����
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
//	//���� ���� �����ε����� ã�� Empty ���·� ��ȯ
//	int idx = -1;
//	for (int i = 0; i < WeaponSlots.Num(); i++)
//	{
//		//����Ÿ�԰� ��ġ�ϴ� ������ idx�� ��ȯ
//		if (CurrentWeaponType == WeaponSlots[i])
//		{
//			idx = i;
//			break;
//		}
//	}
//	//��ȯ�� idx���� ������ empty�� ��ȯ
//	WeaponSlots[idx] = EWeaponType::Empty;
//
//	if (CurrentWeaponMesh)
//	{
//		//�÷��̾�Լ� ����Ʈ����
//		CurrentWeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
//		//�浹ü ������ ����ּ� �ٴڿ� �����ְ��ϱ�
//		CurrentWeaponMesh->SetSimulatePhysics(true);
//		CurrentWeaponMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
//	}
//}
//
////�ٴڿ� ������ ���⸦ �����ϴ� �Լ�
//void ATPSPlayer::PickupWeapon(USkeletalMeshComponent* WeaponMesh, EWeaponType NewWeaponType)
//{
//	//�ϴ� �����ִ� ���Ⱑ ������ ��(���⵿�ü��� ����)
//	DropCurrentWeapon();
//
//	//���ο� ���⸦ ���� ����� ����
//	CurrentWeaponType = NewWeaponType;
//	//���ο� ���⸦ ��Ȯ�� ��ġ�� �÷��̾�� ���̱�
//	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket");
//	//������ ������ �浹ü ���� ����(�÷��̾�� �浹����)
//	WeaponMesh->SetSimulatePhysics(false);
//	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
//
//
//	//���� ���� ���Կ� ���ο� ���⸦ ���
//	int idx = -1;
//	for (int i = 0; i < WeaponSlots.Num(); i++)
//	{
//		//���⽽�Կ� Empty�� idx���� ã�Ƽ� ��ȯ
//		if (WeaponSlots[i] == EWeaponType::Empty)
//		{
//			idx = i;
//			break;
//		}
//	}
//	//��ȯ�� idx���� ���ο� ����Ÿ�� ���
//	WeaponSlots[idx] = NewWeaponType;
//	//���⺯���Լ��� ȣ���� ���ο� ������ idx�� ���� �� ����
//	ChangeWeapon(idx); //�ε��� �� ����
//}
//
//

#pragma endregion