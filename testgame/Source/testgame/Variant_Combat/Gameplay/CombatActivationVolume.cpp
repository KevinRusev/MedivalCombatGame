
#include "CombatActivationVolume.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "CombatActivatable.h"

ACombatActivationVolume::ACombatActivationVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	check(Box);

	Box->SetBoxExtent(FVector(500.0f, 500.0f, 500.0f));

	Box->SetCollisionProfileName(FName("OverlapAllDynamic"));

	Box->OnComponentBeginOverlap.AddDynamic(this, &ACombatActivationVolume::OnOverlap);
}

void ACombatActivationVolume::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);

	if (PlayerCharacter)
	{
		
		if (PlayerCharacter->IsPlayerControlled())
		{
			
			for (AActor* CurrentActor : ActorsToActivate)
			{
				
				if(ICombatActivatable* Activatable = Cast<ICombatActivatable>(CurrentActor))
				{
					Activatable->ActivateInteraction(PlayerCharacter);
				}
			}
		}
	}

}