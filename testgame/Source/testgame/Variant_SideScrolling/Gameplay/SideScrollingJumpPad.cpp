
#include "SideScrollingJumpPad.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SceneComponent.h"

ASideScrollingJumpPad::ASideScrollingJumpPad()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(RootComponent);

	Box->SetBoxExtent(FVector(115.0f, 90.0f, 20.0f), false);
	Box->SetRelativeLocation(FVector(0.0f, 0.0f, 16.0f));

	Box->SetCollisionObjectType(ECC_WorldDynamic);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	OnActorBeginOverlap.AddDynamic(this, &ASideScrollingJumpPad::BeginOverlap);
}

void ASideScrollingJumpPad::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	
	if (ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor))
	{
		
		OverlappingCharacter->Jump();

		FVector LaunchVelocity = FVector::UpVector * ZStrength;
		OverlappingCharacter->LaunchCharacter(LaunchVelocity, false, true);
	}
}
