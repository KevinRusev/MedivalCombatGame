
#include "PickupItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(50.f);
	
	CollisionSphere->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	RootComponent = CollisionSphere;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bCanBePickedUp = true;
	RotationSpeed = 90.f;
	BobAmplitude = 10.f;
	BobSpeed = 2.f;
	BobTime = 0.f;
	bIsHighlighted = false;
}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
}

void APickupItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanBePickedUp)
	{
		
		FRotator NewRotation = GetActorRotation();
		NewRotation.Yaw += RotationSpeed * DeltaTime;
		SetActorRotation(NewRotation);

		BobTime += DeltaTime * BobSpeed;
		FVector NewLocation = StartLocation;
		NewLocation.Z += FMath::Sin(BobTime) * BobAmplitude;
		SetActorLocation(NewLocation);

		if (bIsHighlighted && ItemMesh)
		{
			float PulseValue = (FMath::Sin(BobTime * 4.0f) + 1.0f) * 0.5f;
			float Scale = 1.2f + (PulseValue * 0.2f);
			ItemMesh->SetRelativeScale3D(FVector(Scale, Scale, Scale));
		}
	}
}

void APickupItem::OnPickedUp_Implementation(AActor* PickingActor)
{
	
	bCanBePickedUp = false;
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	SetLifeSpan(0.1f);
}

void APickupItem::SetHighlighted(bool bHighlight)
{
	if (bIsHighlighted == bHighlight)
	{
		return;
	}

	bIsHighlighted = bHighlight;

	if (ItemMesh)
	{
		
		ItemMesh->SetRenderCustomDepth(bHighlight);
		ItemMesh->SetCustomDepthStencilValue(bHighlight ? 1 : 0);

		if (bHighlight)
		{
			ItemMesh->SetRelativeScale3D(FVector(1.3f, 1.3f, 1.3f));
		}
		else
		{
			ItemMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
		}
	}
}
