
#include "SideScrollingSoftPlatform.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "SideScrollingCharacter.h"

ASideScrollingSoftPlatform::ASideScrollingSoftPlatform()
{
 	PrimaryActorTick.bCanEverTick = true;

	RootComponent = Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_WorldStatic);
	Mesh->SetCollisionResponseToAllChannels(ECR_Block);

	CollisionCheckBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Check Box"));
	CollisionCheckBox->SetupAttachment(Mesh);

	CollisionCheckBox->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
	CollisionCheckBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionCheckBox->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionCheckBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionCheckBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	CollisionCheckBox->OnComponentBeginOverlap.AddDynamic(this, &ASideScrollingSoftPlatform::OnSoftCollisionOverlap);
}

void ASideScrollingSoftPlatform::OnSoftCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (ASideScrollingCharacter* Char = Cast<ASideScrollingCharacter>(OtherActor))
	{
		
		Char->SetSoftCollision(true);
	}
}

void ASideScrollingSoftPlatform::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (ASideScrollingCharacter* Char = Cast<ASideScrollingCharacter>(OtherActor))
	{
		
		Char->SetSoftCollision(false);
	}
}
