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

	//�������۸�忡�� Ÿ�Ӷ��� ����
	//Ÿ�Ӷ��� ������Ʈ �ʱ�ȭ
	ZoomTimeLine = CreateDefaultSubobject<UTimelineComponent>(TEXT("ZoomTimeLine"));
	//���� ��ġ �� ȸ�� �ʱ�ȭ
	OriginalGunTransform = FTransform::Identity;
	//���ϴ� ��ǥ ��ġ �� ȸ������ ����
	TargetGunTransform = FTransform(FRotator(7.0f, 5.0f, 3.0f), FVector(15.0f, -70.0f, 23.0f)); 
     

#pragma endregion
	
#pragma region Movement Settings

	//2�� ���� ����
	JumpMaxCount = 2;
	//�ʱ� ���¸� ��� �������� ����
	bCanLaunch = true;
	//�⺻�����»��� ���� �ʱ�ȭ
	OriginalGroundFriction = GetCharacterMovement()->GroundFriction;

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
	BaseGrenadeDamage = 150.0f;

	//����ź �⺻����
	ThrowingForce = 800.0f;

#pragma endregion

}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();

#pragma region UI Setting

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

	// �������� UI ���� �ν��Ͻ� ����
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);

	// Ÿ�Ӷ��� ��� �����Ǿ��ִ��� Ȯ��
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

#pragma endregion

#pragma region Enhanced Input Setting

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
#pragma endregion

	//���� �������̺�Ʈ ó���Լ� ���ε�
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ATPSPlayer::OnMyOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ATPSPlayer::OnMyOverlapEnd);

	//�ʱ� �÷��̾� ���� ����
	tpsCamComp->SetActive(!bIsFirstPerson);
	fpsCamComp->SetActive(bIsFirstPerson);

	//�ʱ� ���� ������ ���� �޽� ���ü� ����
	GetMesh()->SetVisibility(!bIsFirstPerson, true);
	FPSMesh->SetVisibility(bIsFirstPerson, true);

#pragma region Weapon Default Setting

	//Weapon Factory ���
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

	//WeaponFactory���� ������ ���⸦ �����Ѵ�.
	EquippedWeapons.Add(CurrentWeaponActor);
	EquippedWeapons.Add(SecondWeaponActor);
	EquippedWeapons.Add(ThirdWeaponActor);

	//�����Ҷ� 0�� ���� ����� ����(1��Ű)
	ChangeWeapon(0);

	//�����ҋ� ��ź���� �Ⱥ��̰� ó��
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

	//PlayerHUD UI������Ʈ
	UpdateMyHUD();

	//ī�޶� ��ġ�� �Ӹ� ��ġ�� ���� ����
	FVector newCameraLocation = GetMesh()->GetSocketLocation(TEXT("headSocket"));
	fpsCamComp->SetWorldLocation(newCameraLocation);

	UpdateAimingEnemy();
}

void ATPSPlayer::UpdateAimingEnemy()
{
	// ī�޶� ������
	UCameraComponent* CameraComponent = bIsFirstPerson ? fpsCamComp : tpsCamComp;
	if (CameraComponent == nullptr)
	{
		return;
	}

	// ����Ʈ���̽� ���� ��ġ�� ���� ����
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = Start + (CameraComponent->GetForwardVector() * 5000);

	// ����Ʈ���̽� ��� ������ ���� �迭
	FHitResult HitResult;

	// ����Ʈ���̽� �Ű����� ����
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// ����Ʈ���̽� ����
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		QueryParams
	);

	// ����׿� ����Ʈ���̽� �ð�ȭ
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

				// ���� ������ ���� �����ϰ� ǥ��
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

	//���콺 ��Ʈ�� �̺�Ʈ ó���Լ� ���ε�
	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if(PlayerInput)
	{
		//������Ʈ���� �Է¹��ε��� ó���ϵ��� �����Լ� ȣ��
		playerMove->SetupInputBinding(PlayerInput);
	}
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
		
		AmmoText->SetText(FText::AsNumber(CurrentAmmo));
	}
	if (MaxAmmoText)
	{
		//MaxAmmoText�� ���� ���� ������ ������ Max ź����� ���ε�
		int32 MaxAmmo = CurrentWeaponActor->MaxWeaponAmmo;
		
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

//������� �̺�Ʈó���Լ�
void ATPSPlayer::GainWeapon()
{
	if (nullptr != OverlappedWeapon && ! IsDetaching)
	{
		// ������ ���Ⱑ �ִٸ� ������.
		if (!CurrentWeaponActor->IsA<AMyEmptyWeaponActor>())
		{
			CurrentWeaponActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			//Detach�Ҷ� �浹ó���� �ٷ� �����Ǹ� ������ �ڲ� ���ܼ� ����ó���ϰ� �Լ�ȣ��� ����.
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
		//���� ���������� ���� ���⽽���� ���⸦ ä���.
		EquippedWeapons[CurrentWeaponIndex] = OverlappedWeapon;
		CurrentWeaponActor = OverlappedWeapon;
		
		//���⸦ Attach���ش�.
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		CurrentWeaponActor->AttachToComponent(FPSMesh, Rules, TEXT("GripPoint"));
		CurrentWeaponActor->WeaponBoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CurrentWeaponActor->WeaponSpawnEffect->SetActive(false);
		CurrentWeaponActor->SetCamera(fpsCamComp);
		CurrentWeaponActor->SetPlayerBuff(AttackPower);

		OverlappedWeapon = nullptr;

		//Attach�Ҹ��� �÷����Ѵ�.
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponAttachSound);
	}
}
//������ �̺�Ʈó���Լ�
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

//��������� �̺�Ʈó���Լ�
void ATPSPlayer::OnMyOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

#pragma region Dash Event (LaunchCharacter)

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

		//�뽬�Ҹ��� �÷����Ѵ�.
		UGameplayStatics::PlaySound2D(GetWorld(), PlayerDashSound);

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

#pragma region Weapon(Direct Control) Settings

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
	CurrentWeaponActor->WeaponStopAttack();
}

#pragma endregion
