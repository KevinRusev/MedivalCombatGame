
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatActivatable.h"
#include "CombatEnemySpawner.generated.h"

class UCapsuleComponent;
class UArrowComponent;
class ACombatEnemy;

UCLASS(abstract)
class ACombatEnemySpawner : public AActor, public ICombatActivatable
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* SpawnCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* SpawnDirection;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Spawner")
	TSubclassOf<ACombatEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Spawner")
	bool bShouldSpawnEnemiesImmediately = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Spawner", meta = (ClampMin = 0, ClampMax = 10))
	float InitialSpawnDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Spawner", meta = (ClampMin = 0, ClampMax = 100))
	int32 SpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Enemy Spawner", meta = (ClampMin = 0, ClampMax = 10))
	float RespawnDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Activation", meta = (ClampMin = 0, ClampMax = 10))
	float ActivationDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Activation")
	TArray<AActor*> ActorsToActivateWhenDepleted;

	bool bHasBeenActivated = false;

	FTimerHandle SpawnTimer;

public:
	
	ACombatEnemySpawner();

public:

	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

protected:

	void SpawnEnemy();

	UFUNCTION()
	void OnEnemyDied();

	void SpawnerDepleted();

public:

	UFUNCTION(BlueprintCallable, Category="Activatable")
	virtual void ToggleInteraction(AActor* ActivationInstigator) override;

	UFUNCTION(BlueprintCallable, Category="Activatable")
	virtual void ActivateInteraction(AActor* ActivationInstigator) override;

	UFUNCTION(BlueprintCallable, Category="Activatable")
	virtual void DeactivateInteraction(AActor* ActivationInstigator) override;

};
