// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "EnemyAIController.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "AnimInstance/EnemyAnimInstance.h"
#include "Projectile/Projectile.h"
#include "Bullet.h"
#include "Component/EnemyStatComponent.h"
#include "Engine/DamageEvents.h"
#include "Widget/EnemyWidget.h"
#include "Components/WidgetComponent.h"
#include "PlayerWeapon/MyWeaponActor.h"

#include "NiagaraSystem.h" //추가
#include "NiagaraComponent.h" //추가
#include "NiagaraFunctionLibrary.h" //추가
#include "Widget/DamageAmt.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	EnemyHPStat = CreateDefaultSubobject<UEnemyStatComponent>(TEXT("STAT"));

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
	HPBarWidget->SetupAttachment(GetMesh());

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("WidgetBlueprint'/Game/BluePrints/Enemy/BP_EnemyWidget.BP_EnemyWidget_C'"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));

	    // 비지블 취소
		HPBarWidget->SetVisibility(false);
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	auto CharacterWidget = Cast<UEnemyWidget>(HPBarWidget->GetUserWidgetObject());
	if (nullptr != CharacterWidget && EnemyHPStat)
	{
		CharacterWidget->BindEnemyStat(EnemyHPStat);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MyAnim = Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance());

	if (MyAnim)
	{
		MyAnim->OnMontageEnded.AddDynamic(this, &AEnemy::AttackEnd);

		MyAnim->OnAttack.AddLambda([this]() -> void {
			OnAttack();
			});
	}

	if (EnemyHPStat != nullptr)
	{
		EnemyHPStat->OnHPIsZero.AddLambda([this]()->void {
			// MyAnim->SetDeadAnim();
			// SetActorEnableCollision(false);
			HPBarWidget->SetVisibility(false);
			HPBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			// 충돌체 제거
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			if (DropWeaponFactory != nullptr)
			{
				auto* WeaponActor = GetWorld()->SpawnActor<AMyWeaponActor>(DropWeaponFactory);
				WeaponActor->SetActorLocation(GetActorLocation());
				WeaponActor->WeaponBoxComp->SetSimulatePhysics(true);
				WeaponActor->WeaponBoxComp->AddForce(FVector(0, 0, 10000));
				WeaponActor->WeaponSpawnEffect->SetActive(true);
				WeaponActor->WeaponBoxComp->SetCollisionProfileName(TEXT("Weapon"));
			}
			});
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		FHitResult result = PointDamageEvent->HitInfo;
		if (result.Component.IsValid())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = nullptr;
			
			if (EnemyHPStat != nullptr && !IsDead())
			{
				if (result.Component->ComponentHasTag("Head"))
				{
					auto* DamageAmt = GetWorld()->SpawnActor<ADamageAmt>(CriticalDamageAmtFactory, result.Component->GetComponentLocation(), FRotator::ZeroRotator);
					FinalDamage *= 2;
					DamageAmt->SetDamageText(FinalDamage, true);
				}
				else
				{
					auto* DamageAmt = GetWorld()->SpawnActor<ADamageAmt>(DamageAmtFactory, GetActorLocation(), FRotator::ZeroRotator);
					DamageAmt->SetDamageText(FinalDamage, false);
				}

				HPBarWidget->SetVisibility(true);
				EnemyHPStat->SetDamage(FinalDamage);
			}
		}
		else
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = nullptr;
			auto* DamageAmt = GetWorld()->SpawnActor<ADamageAmt>(DamageAmtFactory, GetActorLocation(), FRotator::ZeroRotator);
			HPBarWidget->SetVisibility(true);
			EnemyHPStat->SetDamage(FinalDamage);
			DamageAmt->SetDamageText(FinalDamage, false);
		}
	}
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = nullptr;
		auto* DamageAmt = GetWorld()->SpawnActor<ADamageAmt>(DamageAmtFactory, GetActorLocation(), FRotator::ZeroRotator);
		HPBarWidget->SetVisibility(true);
		EnemyHPStat->SetDamage(FinalDamage);
		DamageAmt->SetDamageText(FinalDamage, false);
	}

	return FinalDamage;
}

void AEnemy::DoAttack()
{
	MyAnim->PlayMontage();
	/*FTimerHandle deathTimer;
	GetWorld()->GetTimerManager().SetTimer(deathTimer, this, &AEnemy::AttackEnd, 0.5f, false);*/
}

void AEnemy::OnAttack()
{
}

void AEnemy::AttackEnd(UAnimMontage* Montage, bool bInterrupted)
{
	OnAttackEnd.Broadcast();
}

bool AEnemy::IsDead() const
{
	if (EnemyHPStat)
	{
		return EnemyHPStat->IsDead();
	}
	else
	{
		return false;
	}
}

void AEnemy::AimEnemy(bool bAiming)
{
}
