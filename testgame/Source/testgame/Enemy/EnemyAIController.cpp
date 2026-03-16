
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

	RepathInterval = 0.25f;
	RepathMoveThreshold = 200.f;
	LastRepathTime = -1000.f;
	LastRequestedTargetLocation = FVector::ZeroVector;
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

	if (ControlledEnemy->IsAttacking() || ControlledEnemy->IsHitReacting())
	{
		return;
	}

	AActor* Target = ControlledEnemy->GetTargetActor();

	if (!Target)
	{
		if (bIsChasing)
		{
			StopChasing();
		}
		ClearFocus(EAIFocusPriority::Gameplay);
		return;
	}

	const bool bCanSeeTarget = LineOfSightTo(Target);

	if (bCanSeeTarget)
	{
		LoseSightTimer = 0.f;

		if (!bIsChasing)
		{
			ChaseTarget(Target);
		}
		else
		{
			
			const float Now = GetWorld()->GetTimeSeconds();
			const float DistSq = FVector::DistSquared(Target->GetActorLocation(), LastRequestedTargetLocation);
			if ((Now - LastRepathTime) >= RepathInterval ||
				DistSq >= FMath::Square(RepathMoveThreshold))
			{
				MoveToActor(Target, AcceptanceRadius);
				LastRepathTime = Now;
				LastRequestedTargetLocation = Target->GetActorLocation();
			}
		}

		SetFocus(Target, EAIFocusPriority::Gameplay);
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);

		LoseSightTimer += DeltaTime;

		if (LoseSightTimer >= LoseSightDuration)
		{
			StopChasing();
			ControlledEnemy->SetTargetActor(nullptr);
		}
		else if (bIsChasing)
		{
			
			const float Now = GetWorld()->GetTimeSeconds();
			if ((Now - LastRepathTime) >= RepathInterval)
			{
				MoveToActor(Target, AcceptanceRadius);
				LastRepathTime = Now;
				LastRequestedTargetLocation = Target->GetActorLocation();
			}
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

	MoveToActor(Target, AcceptanceRadius);
	LastRepathTime = GetWorld()->GetTimeSeconds();
	LastRequestedTargetLocation = Target->GetActorLocation();

	UE_LOG(LogTemp, Log, TEXT("Enemy started chasing %s"), *Target->GetName());
}

void AEnemyAIController::StopChasing()
{
	bIsChasing = false;
	StopMovement();
	ClearFocus(EAIFocusPriority::Gameplay);

	UE_LOG(LogTemp, Log, TEXT("Enemy stopped chasing"));
}
