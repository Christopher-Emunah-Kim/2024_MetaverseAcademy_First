// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ObjectMacros.h" // �߰�
#include "UObject/Class.h" //�߰�
#include "Components/TimelineComponent.h" //�߰�
#include "NiagaraSystem.h" //�߰�
#include "NiagaraComponent.h" //�߰�
#include "NiagaraFunctionLibrary.h" //�߰�
#include "TPSPlayer.generated.h"


//���� ����Ÿ���� ��Ÿ���� ������ ����
//UENUM(BlueprintType)
//enum class EWeaponType : uint8
//{
//	Empty,
//	SMG,
//	SniperGun,
//	GrenadeGun,
//	ShotGun,
//	ShotGunWeaponActor,
//};

UCLASS()
class GUNFIRE_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//������ ó�� �Լ�
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:

#pragma region Component, Child Class Setting

	//�̵�ó�� ������Ʈ ���̱�
	UPROPERTY(VisibleAnywhere, Category = Component)
	class UMyPlayerBaseComponent* playerMove;

	//ShotGun Actor �ν��Ͻ����常���
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMyWeaponActor> FirstWeaponFactory;

	//SMG Actor �ν��Ͻ� ���� �����
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMyWeaponActor> SecondWeaponFactory;

	//SniperGun Actor �ν��Ͻ� ���� �����
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMyWeaponActor> ThirdWeaponFactory;


	//���� ���� ���� �ν��Ͻ�
	UPROPERTY(EditAnywhere)
	AMyWeaponActor* CurrentWeaponActor;

#pragma endregion

#pragma region Weapon Actor Change, Gain, Detach

	//�������� ������� �׷� ���
	UPROPERTY(EditAnywhere)
	TArray<AMyWeaponActor*> EquippedWeapons;
	//���� ���Ⱑ ���° �ε����� �ִ��� Ȯ���ϴ� ����
	int32 CurrentWeaponIndex = 0;
	//��������Լ� ����
	void GainWeapon();


#pragma endregion

#pragma region Player Setting

	//FPS���� �޽� ������Ʈ �߰�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	USkeletalMeshComponent* FPSMesh;

	//�÷��̾��� ���ݷ� ���� �߰�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSetting)
	float AttackPower;

	//�÷��̾��� ���� HP ���� �߰�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSetting)
	float PlayerHP;

	//�÷��̾��� MAX HP ���� �߰�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSetting)
	float PlayerMAXHP;

#pragma endregion

#pragma region Camera Setting

	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* springArmComp; //������Ʈ �Ӽ� �߰�

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	class UCameraComponent* tpsCamComp; //�÷��̾� TPS ī�޶� �߰�

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	class UCameraComponent* fpsCamComp; //�÷��̾� FPS ī�޶� �߰�


	//1��Ī /3��Ī ���� ��ȯ �Լ�
	void ToggleView();

	//1��Ī �������� ���� üũ ���� ����
	bool bIsFirstPerson;

#pragma endregion

#pragma region Movement Setting

	//Enhanced Input �Ӽ� �߰�
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* imc_TPS;

	//�뽬 �̺�Ʈó�� �Լ� ����
	void InputLaunch();
	//�뽬 ��� �Լ� ����
	void LaunchVelocity();
	//�뽬�ҋ� ���� ��¦ �߰��ϴ� �Լ� ����
	void LaunchUpward();
	//Cooldown�ʱ�ȭ �Լ� ����
	void ResetLaunchCooldown();
	//������ ó���� ������Ʈ ����
	FTimerHandle LaunchUpwardTimeHandle;
	FTimerHandle CooldownTimerHandle;
	FTimerHandle ResetFrictionTimerHandle;
	//�ʱ� ���¸� ��� �������� üũ�� ���� ����
	bool bCanLaunch;
	//�⺻ �����»��� ���� ����
	float OriginalGroundFriction;
	//�����»��� �ʱ�ȭ �Լ� ����
	void ResetGroundFriction();

	// �뽬 ����Ʈ ��ġ�� ���� Scene ������Ʈ ����
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	USceneComponent* DashEffectScene;
	//�뽬 ����Ʈ�� ���� ���̾ư���ý��� ���� ����
	UPROPERTY(EditAnywhere, Category = Effects)
	UNiagaraComponent* PlayerDashEffect;
	//����̺�Ʈ ���̾ư��� ����Ʈ Ÿ�̸�
	FTimerHandle DashEffectDeactivateHandle;

#pragma endregion

#pragma region UI Setting

	//�Ϲ����ظ�� ũ�ν����UI ���� ���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UUserWidget> crosshairUIFactory;
	//ũ�ν���� UI ���� �ν��Ͻ�
	UPROPERTY()
	class UUserWidget* _crosshairUI;

	//�������� UI ���� ���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UUserWidget> sniperUIFactory;
	//�������� UI ���� �ν��Ͻ�

	UPROPERTY()
	class UUserWidget* _sniperUI;

	//�������� UI Ÿ�̸� �ڵ� ����
	FTimerHandle SniperUITimerHandle;

	//�÷��̾� HUD���� ���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UUserWidget> playerHUDFactory;

	//�÷��̾� HUD���� �ν��Ͻ�
	UPROPERTY()
	class UUserWidget* _playerHUDUI;

	//�÷��̾� HUD���� ������Ʈ �Լ� ����
	void UpdateMyHUD();

	// ������ UI ���� ���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UUserWidget> TDamageUIFactory;

	// ������ UI ���� �ν��Ͻ�
	UPROPERTY()
	class UUserWidget* _takedamageUI;

	//������ �ε������� �Լ�
	UFUNCTION(BlueprintImplementableEvent)
	void DamageIndicator(FVector enemyLocation);

	//�÷��̾� ��� UI ���� ���� ���� ����
	// Restart UI ���� ���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UUserWidget> RestartUIFactory;

	// Restart UI ���� �ν��Ͻ�
	UPROPERTY()
	class UUserWidget* _restartUI;

	// ������ UI Ÿ�̸�
	FTimerHandle DamagedHandle;

	//�ε� UI ���� ���� ���� 
	UPROPERTY(EditAnywhere, Category = "UI")

    TSubclassOf<class UUserWidget> LoadingUIFactory;

	// �ε� UI ���� �ν��Ͻ�
    UPROPERTY()
    UUserWidget* _loadingUI;

	//�ε��� UI �ϰ� AddtoViewport �Լ�
    void ShowMainUI();

	//�ε� UI Ÿ�̸�
	FTimerHandle LoadingHandle;

#pragma endregion

#pragma region WeaponDefault Setting

	//�Ѿ� �߻�ó�� �Լ�
	void InputFire();

	////�Ѿ�����ȿ�� ���� ���� ����
	//UPROPERTY(EditAnywhere, Category=BulletEffect)
	//class UParticleSystem* bulletEffectFactory;

	////�ѱ�ȭ�� ��ƼŬ�ý��� ���� ����
	//UPROPERTY(EditAnywhere, Category =BulletEffect)
	//class UParticleSystem* MuzzleFlash;

	//����ź�߻� �̺�Ʈó�� �Լ� ����
	void InputGrenade();
    //����ź ���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category=Grenade)
	TSubclassOf<class AGrenade> GrenadeFactory;
    //����ź �߻�ó�� ���� ����
	UPROPERTY(EditDefaultsOnly, Category=Grenade)
	float ThrowingForce;

	//����ź Ÿ�̸� ����
	FTimerHandle GrenadeHandle;

	////������� ���� ���� ����
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	//TArray<EWeaponType> WeaponSlots;

	////���� ���� ���� ���� Ÿ��
	//EWeaponType CurrentWeaponType;

	//���� ���� �Լ� ����
	void ChangeWeapon(int32 slot);

	//������� �̺�Ʈó���Լ� ����
	UFUNCTION()
	void OnMyOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	AMyWeaponActor* OverlappedWeapon;
	
	UFUNCTION()
	void OnMyOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	//���� ������ ���� Ű�Է� �Լ��� ����
	void Weapon1();
	void Weapon2();
	void Weapon3();

	//���� źâ ������ �Լ� ����
	void Reload();
	////���� ������ Ÿ�̸� �ڵ� ����
	//FTimerHandle ReloadTimerHandle;

	////�߻���� Ȯ�� üũ ����
	//bool bIsFiring;

#pragma endregion

#pragma region Weapon Ammo, Damage setting

	////�� ����� ����ź �������� ���� ���� �߰�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float BaseGrenadeGunDamage;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	//float BaseSniperGunDamage;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	//float BaseSMGDamage;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	//float BaseShotGunDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float BaseGrenadeDamage;

	////�� ���⺰ źâ ���� �� ���� ź�෮ ���� ����
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon)
	//int32 MaxSMGAmmo = 70;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon)
	//int32 CurrentSMGAmmo;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon)
	//int32 MaxSniperAmmo = 7;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	//int32 CurrentSniperAmmo;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	//int32 MaxShotGunAmmo = 10;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	//int32 CurrentShotGunAmmo;

#pragma endregion

#pragma region GrenadeGun Setting 
	//�� ���̷�Ż �޽� �߰�
	UPROPERTY(VisibleAnywhere, Category=GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	//�Ѿ˰��� �Ӽ� �߰�
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

#pragma endregion

#pragma region SniperGun Setting

	////�������۰� �޽� �߰�
	//UPROPERTY(VisibleAnywhere, Category=Mesh)
	//class USkeletalMeshComponent* sniperGunComp;

	////������ ����� ���� üũ
	//bool bUsingSniperGun = false;
	//
	////SniperGun �߻�ó���Լ�(LineTrace���)
	//void HandleSniperFire();

	//�������� ���� �Լ�
	void SniperAim();
	//�������� ���������� ���� üũ
	bool bSniperAim = false;

	//Ÿ�Ӷ��� ������Ʈ ����(���ظ�� ���)
	UPROPERTY()
	UTimelineComponent* ZoomTimeLine;

	//Ÿ�Ӷ��� ����� ����
	UPROPERTY(EditAnywhere, Category = Timeline)
	UCurveFloat* ZoomCurve;

	//Ÿ�Ӷ��� ������Ʈ �Լ� ����
	UFUNCTION(BlueprintCallable, Category=Timeline)
	void ZoomTimeLineProgress(float value);

	//�ܸ�� ���� �Լ� ����
	void StartZoom();
	//�ܸ�� ���� �Լ� ����
	void EndZoom();

	//���� ��ġ �� ȸ�� ������ ���� ���� ��ġ ���� ���� ����
	FTransform OriginalGunTransform;

	//�ܸ��� ���� ���� ��ġ/ȸ�� ��ǥ
	FTransform TargetGunTransform;

	////�������� ���� ���̾ư��� �ý��� ���� ����
	//UPROPERTY(EditAnywhere, Category=Effects)
	//UNiagaraSystem* SniperTrail;

	////�������� ���̾ư��� Ÿ�̸�
	//FTimerHandle SniperEffectTimerHandle;

#pragma endregion

#pragma region SMGGun Setting 
	
	//SMG �ѱ� �߰�
	//�߻��Է��Լ��� �״�� InputFire()���
	//�߻������Լ�
	void StopFire();
	////SMG�߻�ӵ�
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SMG)
	//float SMGFireRate;
	////SMG ��Ÿ�
	//UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=SMG)
	//float SMGRange;
	////SMG ���̷�Ż �޽� �߰�
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=GunMesh)
	//USkeletalMeshComponent* SMGMesh;

	////SMG �ڵ��߻�Ÿ�̸��ڵ�
	//FTimerHandle SMGFIreTimer;
	////SMG �߻�ó���Լ�(LineTrace���)
	//void HandleSMGFire();

	////SMG ���� ���̾ư��� �ý��� ���� ����
	//UPROPERTY(EditAnywhere, Category=Effects)
	//UNiagaraSystem* SMGTrail;

	////SMG ���̾ư��� Ÿ�̸�
	//FTimerHandle SMGEffectTimerHandle;

#pragma endregion

#pragma region ShotGun Setting

	////ShotGun ���̷�Ż �޽� �߰�
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = GunMesh)
	//USkeletalMeshComponent* ShotGunMesh;

	////ShotGun ��Ÿ�
	//UPROPERTY(EditAnywhere,BlueprintReadWrite, Category=ShotGun)
	//float ShotGunRange;

	////ShotGun �߻�ó�� �Լ�
	//void HandleShotGunFire();
	//
	////ShotGun ���� ���̾ư��� �ý��� ���� ����
	//UPROPERTY(EditAnywhere, Category=Effects)
	//UNiagaraSystem* ShotGunTrail;

	////ShotGun ���̾ư��� Ÿ�̸�
	//FTimerHandle ShotGunEffectTimerHandle;

#pragma endregion

#pragma region Animation Montage

	//�߻� �ִϸ��̼� ��Ÿ�� ó�� ����
	UPROPERTY(EditDefaultsOnly, Category=Montage)
	class UAnimMontage* FireMontage;

	//���� �ִϸ��̼� ��Ÿ�� ó�� ����
	UPROPERTY(EditDefaultsOnly, Category=Montage)
	class UAnimMontage* ReloadMontage;

#pragma endregion

#pragma region THE GRAVE

	////�ִϸ��̼� �߰��� ���ʿ����� ��鸲 ���� ����
	////�̵��� �޽���鸲ó���Լ� ���� 
	//void ApplyMeshBobbing(float DeltaTime, float BobbingIntensity);
	////�޽� ��鸲 �ʱ���� Ȯ���� ���� ����
	//FVector InitialMeshLocation;

	//��� �̺�Ʈ �߻��� ��鸲 ���� ���� Ÿ�̸� �ڵ�
	//FTimerHandle BobbingIntensityResetTimerHandle;

	////�ִϸ��̼� �߰��� ���ʿ����� �ݵ� �Լ� ����
	////�ݵ��� ���� ī�޶�/���� ������ ��
	//FVector RecoilOffset;

	////�ݵ��� ���� ī�޶�/���� ȸ����
	//FRotator RecoilRotation;

	////�ݵ� �ִϸ��̼� Ÿ�̸� �ڵ�
	//FTimerHandle RecoilTimerHandle;

	//// �߻� ���� �� ī�޶��� Ʈ������ ����
	//FTransform InitialCameraTransform; 

	////�ݵ� �ִϸ��̼� �缳�� �Լ�
	//void ResetRecoil();

	////�ݵ� �ִϸ��̼� ������Ʈ �Լ�
	//void UpdateRecoil(float DeltaTime);

	////�ݵ��� ũ�� ���� �Լ�
	//void ApplyRecoil(FVector RecoilOffset, FRotator RecoilRotation, float RecoilDuration);

	////SMG �߻� �� �޽���ġ ���� Ÿ�̸�
	//FTimerHandle SMGResetTimerHandle;

	////�ڵ��߻�Ǵ� SMG�� ��� ���� �ݵ��ʱ�ȭ
	//void ResetSMGPosition();

	////�ѱ����, ��� ���� ����
	////���� ����ִ� ���⸦ �ٴڿ� ����ϴ� �Լ� ����
	//void DropCurrentWeapon();
	////�ٴ��� ���⸦ �����ϴ� �Լ� ����
	//void PickupWeapon(USkeletalMeshComponent* WeaponMesh, EWeaponType NewWeaponType);

	
#pragma endregion

	
};
