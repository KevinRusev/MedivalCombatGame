
#include "CombatEnemy.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CombatAIController.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "CombatLifeBar.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"

ACombatEnemy::ACombatEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	OnAttackMontageEnded.BindUObject(this, &ACombatEnemy::AttackMontageEnded);

	AIControllerClass = ACombatAIController::StaticClass();

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;

	LifeBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("LifeBar"));
	LifeBar->SetupAttachment(RootComponent);

	GetCapsuleComponent()->SetCapsuleSize(35.0f, 90.0f);

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	CurrentHP = MaxHP;
}

void ACombatEnemy::DoAIComboAttack()
{
	
	if (bIsAttacking)
	{
		return;
	}

	bIsAttacking = true;

	TargetComboCount = FMath::RandRange(1, ComboSectionNames.Num() - 1);

	CurrentComboAttack = 0;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float MontageLength = AnimInstance->Montage_Play(ComboAttackMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);

		if (MontageLength > 0.0f)
		{
			
			AnimInstance->Montage_SetEndDelegate(OnAttackMontageEnded, ComboAttackMontage);
		}
	}
}

void ACombatEnemy::DoAIChargedAttack()
{
	
	if (bIsAttacking)
	{
		return;
	}

	bIsAttacking = true;

	TargetChargeLoops = FMath::RandRange(MinChargeLoops, MaxChargeLoops);

	CurrentChargeLoop = 0;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		const float MontageLength = AnimInstance->Montage_Play(ChargedAttackMontage, 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);

		if (MontageLength > 0.0f)
		{
			
			AnimInstance->Montage_SetEndDelegate(OnAttackMontageEnded, ChargedAttackMontage);
		}
	}
}

void ACombatEnemy::AttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	
	bIsAttacking = false;

	OnAttackCompleted.ExecuteIfBound();
}

void ACombatEnemy::DoAttackTrace(FName DamageSourceBone)
{
	
	TArray<FHitResult> OutHits;

	const FVector TraceStart = GetMesh()->GetSocketLocation(DamageSourceBone);
	const FVector TraceEnd = TraceStart + (GetActorForwardVector() * MeleeTraceDistance);

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(MeleeTraceRadius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->SweepMultiByObjectType(OutHits, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, CollisionShape, QueryParams))
	{
		
		for (const FHitResult& CurrentHit : OutHits)
		{
			
			if (CurrentHit.GetActor()->ActorHasTag(FName("Player")))
			{
				
				ICombatDamageable* Damageable = Cast<ICombatDamageable>(CurrentHit.GetActor());

				if (Damageable)
				{
					
					const FVector Impulse = (CurrentHit.ImpactNormal * -MeleeKnockbackImpulse) + (FVector::UpVector * MeleeLaunchImpulse);

					Damageable->ApplyDamage(MeleeDamage, this, CurrentHit.ImpactPoint, Impulse);

				}
			}
		}
	}
}

void ACombatEnemy::CheckCombo()
{
	
	++CurrentComboAttack;

	if (CurrentComboAttack < TargetComboCount)
	{
		
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_JumpToSection(ComboSectionNames[CurrentComboAttack], ComboAttackMontage);
		}
	}
}

void ACombatEnemy::CheckChargedAttack()
{
	
	++CurrentChargeLoop;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_JumpToSection(CurrentChargeLoop >= TargetChargeLoops ? ChargeAttackSection : ChargeLoopSection, ChargedAttackMontage);
	}
}

void ACombatEnemy::ApplyDamage(float Damage, AActor* DamageCauser, const FVector& DamageLocation, const FVector& DamageImpulse)
{
	
	FDamageEvent DamageEvent;
	const float ActualDamage = TakeDamage(Damage, DamageEvent, nullptr, DamageCauser);

	if (ActualDamage > 0.0f)
	{
		
		GetCharacterMovement()->AddImpulse(DamageImpulse, true);

		if (GetMesh()->IsSimulatingPhysics())
		{
			
			GetMesh()->AddImpulseAtLocation(DamageImpulse * GetMesh()->GetMass(), DamageLocation);
		}

		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(0.1f, ComboAttackMontage);
			AnimInstance->Montage_Stop(0.1f, ChargedAttackMontage);
		}

		ReceivedDamage(ActualDamage, DamageLocation, DamageImpulse.GetSafeNormal());
	}
}

void ACombatEnemy::HandleDeath()
{
	
	LifeBar->SetHiddenInGame(true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->DisableMovement();

	GetMesh()->SetSimulatePhysics(true);

	OnEnemyDied.Broadcast();

	GetWorld()->GetTimerManager().SetTimer(DeathTimer, this, &ACombatEnemy::RemoveFromLevel, DeathRemovalTime);
}

void ACombatEnemy::ApplyHealing(float Healing, AActor* Healer)
{
	
}

void ACombatEnemy::RemoveFromLevel()
{
	
	Destroy();
}

float ACombatEnemy::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	
	if (CurrentHP <= 0.0f)
	{
		return 0.0f;
	}

	CurrentHP -= Damage;

	if (CurrentHP <= 0.0f)
	{
		
		HandleDeath();
	}
	else
	{
		
		LifeBarWidget->SetLifePercentage(CurrentHP / MaxHP);

		GetMesh()->SetPhysicsBlendWeight(0.5f);
		GetMesh()->SetBodySimulatePhysics(PelvisBoneName, false);
	}

	return Damage;
}

void ACombatEnemy::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (CurrentHP >= 0.0f)
	{
		
		GetMesh()->SetPhysicsBlendWeight(0.0f);
	}

	OnEnemyLanded.ExecuteIfBound();
}

void ACombatEnemy::BeginPlay()
{
	
	CurrentHP = MaxHP;

	Super::BeginPlay();

	LifeBarWidget = Cast<UCombatLifeBar>(LifeBar->GetUserWidgetObject());
	check(LifeBarWidget);

	LifeBarWidget->SetLifePercentage(1.0f);
}

void ACombatEnemy::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(DeathTimer);
}
