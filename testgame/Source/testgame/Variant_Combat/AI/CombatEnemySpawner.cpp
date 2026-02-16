
#include "CombatEnemySpawner.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "TimerManager.h"
#include "CombatEnemy.h"

ACombatEnemySpawner::ACombatEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	SpawnCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Spawn Capsule"));
	SpawnCapsule->SetupAttachment(RootComponent);

	SpawnCapsule->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	SpawnCapsule->SetCapsuleSize(35.0f, 90.0f);
	SpawnCapsule->SetCollisionProfileName(FName("NoCollision"));

	SpawnDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("Spawn Direction"));
	SpawnDirection->SetupAttachment(RootComponent);
}

void ACombatEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if (bShouldSpawnEnemiesImmediately)
	{
		
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &ACombatEnemySpawner::SpawnEnemy, InitialSpawnDelay);
	}

}

void ACombatEnemySpawner::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
}

void ACombatEnemySpawner::SpawnEnemy()
{
	
	if (IsValid(EnemyClass))
	{
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ACombatEnemy* SpawnedEnemy = GetWorld()->SpawnActor<ACombatEnemy>(EnemyClass, SpawnCapsule->GetComponentTransform(), SpawnParams);

		if (SpawnedEnemy)
		{
			
			SpawnedEnemy->OnEnemyDied.AddDynamic(this, &ACombatEnemySpawner::OnEnemyDied);
		}
	}
}

void ACombatEnemySpawner::OnEnemyDied()
{
	
	--SpawnCount;

	if (SpawnCount <= 0)
	{
		
		GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &ACombatEnemySpawner::SpawnerDepleted, ActivationDelay);
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &ACombatEnemySpawner::SpawnEnemy, RespawnDelay);
}

void ACombatEnemySpawner::SpawnerDepleted()
{
	
	for (AActor* CurrentActor : ActorsToActivateWhenDepleted)
	{
		
		if (ICombatActivatable* CombatActivatable = Cast<ICombatActivatable>(CurrentActor))
		{
			
			CombatActivatable->ActivateInteraction(this);
		}
	}
}

void ACombatEnemySpawner::ToggleInteraction(AActor* ActivationInstigator)
{
	
}

void ACombatEnemySpawner::ActivateInteraction(AActor* ActivationInstigator)
{
	
	if (bHasBeenActivated || bShouldSpawnEnemiesImmediately)
	{
		return;
	}

	bHasBeenActivated = true;

	SpawnEnemy();
}

void ACombatEnemySpawner::DeactivateInteraction(AActor* ActivationInstigator)
{
	
}
