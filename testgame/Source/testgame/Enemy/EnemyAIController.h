
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

	UFUNCTION(BlueprintCallable, Category = "AI")
	void ChaseTarget(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void StopChasing();

	UFUNCTION(BlueprintCallable, Category = "AI")
	bool IsChasing() const { return bIsChasing; }

protected:
	
	UPROPERTY()
	AEnemyCharacter* ControlledEnemy;

	bool bIsChasing;

	float LoseSightTimer;

	UPROPERTY(EditAnywhere, Category = "AI")
	float LoseSightDuration;

	UPROPERTY(EditAnywhere, Category = "AI")
	float AcceptanceRadius;

	UPROPERTY(EditAnywhere, Category = "AI")
	float RepathInterval;

	UPROPERTY(EditAnywhere, Category = "AI")
	float RepathMoveThreshold;

	float LastRepathTime;

	FVector LastRequestedTargetLocation;
};
