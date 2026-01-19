// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class AEnemyCharacter;

UCLASS()
class TESTGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

public:
	virtual void Tick(float DeltaTime) override;

	/** Start chasing the target */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ChaseTarget(AActor* Target);

	/** Stop chasing and return to patrol */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void StopChasing();

	/** Check if currently chasing a target */
	UFUNCTION(BlueprintCallable, Category = "AI")
	bool IsChasing() const { return bIsChasing; }

protected:
	/** Reference to controlled enemy */
	UPROPERTY()
	AEnemyCharacter* ControlledEnemy;

	/** Is currently chasing */
	bool bIsChasing;

	/** Timer for losing sight of target */
	float LoseSightTimer;

	/** How long to chase after losing sight (seconds) */
	UPROPERTY(EditAnywhere, Category = "AI")
	float LoseSightDuration;

	/** Acceptance radius for movement */
	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptanceRadius;
};
