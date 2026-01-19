// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UPawnSensingComponent;

UCLASS(Blueprintable)
class TESTGAME_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/** Called when enemy sees a pawn */
	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	/** Called when enemy hears a noise */
	UFUNCTION()
	void OnHearNoise(APawn* Instigator, const FVector& Location, float Volume);

	/** Perform attack on target */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack();

	/** Apply damage to the target */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DealDamage();

	/** Get current target */
	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetActor() const { return TargetActor; }

	/** Set target actor */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargetActor(AActor* NewTarget) { TargetActor = NewTarget; }

	/** Check if enemy can attack */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool CanAttack() const;

	/** Check if target is in attack range */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsTargetInRange() const;

protected:
	/** Pawn sensing component for sight and hearing */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UPawnSensingComponent* PawnSensingComp;

	/** Current target to chase/attack */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* TargetActor;

	/** Attack range - how close to be to attack */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange;

	/** Attack damage */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage;

	/** Attack cooldown in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackCooldown;

	/** Movement speed when chasing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ChaseSpeed;

	/** Movement speed when patrolling */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float PatrolSpeed;

	/** Time since last attack */
	float LastAttackTime;

	/** Is currently attacking */
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;
};
