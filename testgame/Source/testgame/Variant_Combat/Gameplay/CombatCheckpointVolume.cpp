
#include "CombatCheckpointVolume.h"
#include "CombatCharacter.h"
#include "CombatPlayerController.h"

ACombatCheckpointVolume::ACombatCheckpointVolume()
{
	
	RootComponent = Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	check(Box);

	Box->SetBoxExtent(FVector(500.0f, 500.0f, 500.0f));

	Box->SetCollisionProfileName(FName("OverlapAllDynamic"));

	Box->OnComponentBeginOverlap.AddDynamic(this, &ACombatCheckpointVolume::OnOverlap);
}

void ACombatCheckpointVolume::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (bCheckpointUsed)
	{
		return;
	}
		
	ACombatCharacter* PlayerCharacter = Cast<ACombatCharacter>(OtherActor);

	if (PlayerCharacter)
	{
		if (ACombatPlayerController* PC = Cast<ACombatPlayerController>(PlayerCharacter->GetController()))
		{
			
			bCheckpointUsed = true;

			PC->SetRespawnTransform(PlayerCharacter->GetActorTransform());
		}

	}
}
