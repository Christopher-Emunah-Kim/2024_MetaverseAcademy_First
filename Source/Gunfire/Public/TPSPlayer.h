// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ObjectMacros.h" // 추가
#include "UObject/Class.h" //추가
#include "Components/TimelineComponent.h" //추가
#include "NiagaraSystem.h" //추가
#include "NiagaraComponent.h" //추가
#include "NiagaraFunctionLibrary.h" //추가
#include "TPSPlayer.generated.h"


//현재 무기타입을 나타내는 열거형 변수
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

	//데미지 처리 함수
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	void UpdateAimingEnemy();
	class AEnemy* DetectedEnemy;


#pragma region Player Component Class Setting

	//이동처리 컴포넌트 붙이기
	UPROPERTY(VisibleAnywhere, Category = Component)
	class UMyPlayerBaseComponent* playerMove;

#pragma endregion

#pragma region Weapon Actor Change, Gain, Detach

	//WeaponActor 인스턴스공장만들기(1)
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMyWeaponActor> FirstWeaponFactory;

	//WeaponActor 인스턴스공장만들기(2)
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMyWeaponActor> SecondWeaponFactory;

	//WeaponActor 인스턴스공장만들기(3)
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AMyWeaponActor> ThirdWeaponFactory;

	//현재 무기 액터 인스턴스
	UPROPERTY(EditAnywhere)
	AMyWeaponActor* CurrentWeaponActor;

	//장착중인 무기액터 그룹 기억
	UPROPERTY(EditAnywhere)
	TArray<AMyWeaponActor*> EquippedWeapons;

	//현재 무기가 몇번째 인덱스에 있는지 확인하는 변수
	int32 CurrentWeaponIndex = 0;

	//무기습득함수 선언
	void GainWeapon();
	void DetachWeapon();
	bool IsDetaching = false;
	FTimerHandle DetachTimerHandle;

	//드랍된 무기 변수 인스턴스
	UPROPERTY(EditAnywhere)
	AMyWeaponActor* DetachWeaponActor;

	//무기 변경 함수 선언
	void ChangeWeapon(int32 slot);

	//무기오버랩 이벤트처리함수
	UFUNCTION()
	void OnMyOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnMyOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	//오버랩이벤트 WeaponActor 인스턴스
	AMyWeaponActor* OverlappedWeapon;


	//무기 변경을 위한 키입력 함수들 선언
	void Weapon1();
	void Weapon2();
	void Weapon3();

	//무기 탄창 재장전 함수 선언
	void Reload();

#pragma endregion

#pragma region Player Setting

	//FPS전용 메시 컴포넌트 추가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	USkeletalMeshComponent* FPSMesh;

	//플레이어의 공격력 스탯 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSetting)
	float AttackPower;

	//플레이어의 현재 HP 스탯 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSetting)
	float PlayerHP;

	//플레이어의 MAX HP 스탯 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSetting)
	float PlayerMAXHP;

#pragma endregion

#pragma region Camera Setting

	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* springArmComp; //컴포넌트 속성 추가

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	class UCameraComponent* tpsCamComp; //플레이어 TPS 카메라 추가

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	class UCameraComponent* fpsCamComp; //플레이어 FPS 카메라 추가


	//1인칭 /3인칭 시점 전환 함수
	void ToggleView();

	//1인칭 시점인지 여부 체크 저장 변수
	bool bIsFirstPerson;

#pragma endregion

#pragma region Movement Setting

	//Enhanced Input 속성 추가
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* imc_TPS;

	//대쉬 이벤트처리 함수 선언
	void InputLaunch();
	//대쉬 기능 함수 선언
	void LaunchVelocity();
	//대쉬할떄 위로 살짝 뜨게하는 함수 선언
	void LaunchUpward();
	//Cooldown초기화 함수 선언
	void ResetLaunchCooldown();
	//딜레이 처리할 변수세트 선언
	FTimerHandle LaunchUpwardTimeHandle;
	FTimerHandle CooldownTimerHandle;
	FTimerHandle ResetFrictionTimerHandle;
	//초기 상태를 대시 가능으로 체크할 변수 선언
	bool bCanLaunch;
	//기본 마찰력상태 변수 선언
	float OriginalGroundFriction;
	//마찰력상태 초기화 함수 선언
	void ResetGroundFriction();

	// 대쉬 이펙트 위치를 위한 Scene 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	USceneComponent* DashEffectScene;
	//대쉬 이펙트를 위한 나이아가라시스템 변수 선언
	UPROPERTY(EditAnywhere, Category = Effects)
	UNiagaraComponent* PlayerDashEffect;
	//대시이벤트 나이아가라 이펙트 타이머
	FTimerHandle DashEffectDeactivateHandle;

#pragma endregion

#pragma region UI Setting

	//일반조준모드 크로스헤어UI 위젯 공장 변수 선언
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UUserWidget> crosshairUIFactory;

	//크로스헤어 UI 위젯 인스턴스
	UPROPERTY()
	class UUserWidget* _crosshairUI;

	//스나이퍼 UI 위젯 공장 변수 선언
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UUserWidget> sniperUIFactory;

	//스나이퍼 UI 위젯 인스턴스
	UPROPERTY()
	class UUserWidget* _sniperUI;

	//스나이퍼 UI 타이머 핸들 선언
	FTimerHandle SniperUITimerHandle;

	//플레이어 HUD위젯 공장 변수 선언
	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UUserWidget> playerHUDFactory;

	//플레이어 HUD위젯 인스턴스
	UPROPERTY()
	class UUserWidget* _playerHUDUI;

	//플레이어 HUD위젯 업데이트 함수 선언
	void UpdateMyHUD();

	// 데미지 UI 위젯 공장 변수 선언
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UUserWidget> TDamageUIFactory;

	// 데미지 UI 위젯 인스턴스
	UPROPERTY()
	class UUserWidget* _takedamageUI;

	//데미지 인디케이터 함수
	UFUNCTION(BlueprintImplementableEvent)
	void DamageIndicator(FVector enemyLocation);

	//플레이어 사망 UI 위젯 공장 변수 선언
	// Restart UI 위젯 공장 변수 선언
	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UUserWidget> RestartUIFactory;

	// Restart UI 위젯 인스턴스
	UPROPERTY()
	class UUserWidget* _restartUI;

	// 데미지 UI 타이머
	FTimerHandle DamagedHandle;

	//로딩 UI 위젯 공장 변수 
	UPROPERTY(EditAnywhere, Category = "UI")

    TSubclassOf<class UUserWidget> LoadingUIFactory;

	// 로딩 UI 위젯 인스턴스
    UPROPERTY()
    UUserWidget* _loadingUI;

	//로딩후 UI 일괄 AddtoViewport 함수
    void ShowMainUI();

	//로딩 UI 타이머
	FTimerHandle LoadingHandle;

#pragma endregion

#pragma region WeaponDefault Setting

	//총알 발사처리 함수
	void InputFire();

	//발사중지함수
	void StopFire();

	//수류탄발사 이벤트처리 함수 선언
	void InputGrenade();

    //수류탄 공장 변수 선언
	UPROPERTY(EditDefaultsOnly, Category=Grenade)
	TSubclassOf<class AGrenade> GrenadeFactory;

    //수류탄 발사처리 변수 선언
	UPROPERTY(EditDefaultsOnly, Category=Grenade)
	float ThrowingForce;

	//수류탄 타이머 설정
	FTimerHandle GrenadeHandle;

	//수류탄 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float BaseGrenadeDamage;

	//유탄총 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float BaseGrenadeGunDamage;

	//유탄총 스켈레탈 메시 추가
	UPROPERTY(VisibleAnywhere, Category=GunMesh)
	class USkeletalMeshComponent* gunMeshComp;

	//유탄총 총알공장 속성 추가
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;

#pragma endregion

#pragma region Sniper Scope Mode Setting

	//스나이퍼 조준 함수
	void SniperAim();
	//스나이퍼 조준중인지 여부 체크
	bool bSniperAim = false;

	//타임라인 컴포넌트 선언(조준모드 사용)
	UPROPERTY()
	UTimelineComponent* ZoomTimeLine;

	//타임라인 곡선변수 선언
	UPROPERTY(EditAnywhere, Category = Timeline)
	UCurveFloat* ZoomCurve;

	//타임라인 업데이트 함수 선언
	UFUNCTION(BlueprintCallable, Category=Timeline)
	void ZoomTimeLineProgress(float value);

	//줌모드 시작 함수 선언
	void StartZoom();
	//줌모드 종료 함수 선언
	void EndZoom();

	//무기 위치 및 회전 조정을 위한 원래 위치 저장 변수 선언
	FTransform OriginalGunTransform;

	//줌모드시 변경 무기 위치/회전 목표
	FTransform TargetGunTransform;

#pragma endregion

#pragma region Animation Montage Setting

	//발사 애니메이션 몽타주 처리 변수
	UPROPERTY(EditDefaultsOnly, Category=Montage)
	class UAnimMontage* FireMontage;

	//장전 애니메이션 몽타주 처리 변수
	UPROPERTY(EditDefaultsOnly, Category=Montage)
	class UAnimMontage* ReloadMontage;

#pragma endregion

#pragma region Sound Setting

	//Player Dash Sound 
	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundBase* PlayerDashSound;

	//Player Weapon Attach Sound 
	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundBase* WeaponAttachSound;

#pragma endregion

};
