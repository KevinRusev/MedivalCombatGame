// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyAIController::AEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsChasing = false;
	LoseSightTimer = 0.f;
	LoseSightDuration = 5.f;
	AcceptanceRadius = 100.f;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledEnemy = Cast<AEnemyCharacter>(InPawn);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!ControlledEnemy)
	{
		return;
	}

	AActor* Target = ControlledEnemy->GetTargetActor();

	if (Target)
	{
		// Check line of sight
		bool bCanSeeTarget = LineOfSightTo(Target);

		if (bCanSeeTarget)
		{
			// Reset lose sight timer when we can see target
			LoseSightTimer = 0.f;
			
			if (!bIsChasing)
			{
				ChaseTarget(Target);
			}
			else
			{
				// Update movement destination
				MoveToActor(Target, AcceptanceRadius);
			}
		}
		else
		{
			// Can't see target, start counting
			LoseSightTimer += DeltaTime;

			if (LoseSightTimer >= LoseSightDuration)
			{
				// Lost target for too long, give up
				StopChasing();
				ControlledEnemy->SetTargetActor(nullptr);
			}
			else if (bIsChasing)
			{
				// Keep moving to last known position
				MoveToActor(Target, AcceptanceRadius);
			}
		}
	}
	else
	{
		// No target, stop chasing
		if (bIsChasing)
		{
			StopChasing();
		}
	}
}

void AEnemyAIController::ChaseTarget(AActor* Target)
{
	if (!Target || !ControlledEnemy)
	{
		return;
	}

	bIsChasing = true;
	LoseSightTimer = 0.f;

	// Start moving to target
	MoveToActor(Target, AcceptanceRadius);

	UE_LOG(LogTemp, Log, TEXT("Enemy started chasing %s"), *Target->GetName());
}

void AEnemyAIController::StopChasing()
{
	bIsChasing = false;
	StopMovement();

	UE_LOG(LogTemp, Log, TEXT("Enemy stopped chasing"));
}
