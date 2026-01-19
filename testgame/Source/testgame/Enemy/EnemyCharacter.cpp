// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnemyCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create pawn sensing component
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	PawnSensingComp->SetPeripheralVisionAngle(60.f);
	PawnSensingComp->SightRadius = 2000.f;
	PawnSensingComp->HearingThreshold = 600.f;
	PawnSensingComp->LOSHearingThreshold = 1200.f;

	// Default values
	AttackRange = 200.f;
	AttackDamage = 20.f;
	AttackCooldown = 1.5f;
	ChaseSpeed = 450.f;
	PatrolSpeed = 150.f;
	LastAttackTime = -AttackCooldown; // Allow immediate first attack
	bIsAttacking = false;
	TargetActor = nullptr;
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind sensing events
	if (PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AEnemyCharacter::OnSeePawn);
		PawnSensingComp->OnHearNoise.AddDynamic(this, &AEnemyCharacter::OnHearNoise);
	}

	// Set initial speed
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	}
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Auto-attack if target is in range
	if (TargetActor && IsTargetInRange() && CanAttack())
	{
		Attack();
	}
}

void AEnemyCharacter::OnSeePawn(APawn* Pawn)
{
	// Check if it's the player
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && Pawn == PC->GetPawn())
	{
		TargetActor = Pawn;
		
		// Speed up when chasing
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		}
	}
}

void AEnemyCharacter::OnHearNoise(APawn* Instigator, const FVector& Location, float Volume)
{
	// Could be used to investigate sounds
	// For now, just set target if we hear the player
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && Instigator == PC->GetPawn())
	{
		TargetActor = Instigator;
	}
}

void AEnemyCharacter::Attack()
{
	if (!CanAttack())
	{
		return;
	}

	bIsAttacking = true;
	LastAttackTime = GetWorld()->GetTimeSeconds();

	// Deal damage after a short delay (for animation timing)
	// In a real game, you'd trigger this from an animation notify
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyCharacter::DealDamage, 0.3f, false);

	// Reset attacking state after cooldown
	FTimerHandle ResetTimerHandle;
	GetWorldTimerManager().SetTimer(ResetTimerHandle, [this]()
	{
		bIsAttacking = false;
	}, 0.5f, false);
}

void AEnemyCharacter::DealDamage()
{
	if (!TargetActor || !IsTargetInRange())
	{
		return;
	}

	// Apply damage to target
	FDamageEvent DamageEvent;
	TargetActor->TakeDamage(AttackDamage, DamageEvent, GetController(), this);

	UE_LOG(LogTemp, Log, TEXT("Enemy dealt %f damage to %s"), AttackDamage, *TargetActor->GetName());
}

bool AEnemyCharacter::CanAttack() const
{
	if (bIsAttacking)
	{
		return false;
	}

	float TimeSinceLastAttack = GetWorld()->GetTimeSeconds() - LastAttackTime;
	return TimeSinceLastAttack >= AttackCooldown;
}

bool AEnemyCharacter::IsTargetInRange() const
{
	if (!TargetActor)
	{
		return false;
	}

	float Distance = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
	return Distance <= AttackRange;
}
