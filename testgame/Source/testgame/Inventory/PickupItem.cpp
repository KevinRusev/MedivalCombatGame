// Copyright Epic Games, Inc. All Rights Reserved.

#include "PickupItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create collision sphere - this is used for line trace detection
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(50.f);
	// Use BlockAllDynamic so line traces can hit it
	CollisionSphere->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionSphere->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	RootComponent = CollisionSphere;

	// Create item mesh
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	// Mesh doesn't need collision since sphere handles it
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Default values
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
		// Rotate the item
		FRotator NewRotation = GetActorRotation();
		NewRotation.Yaw += RotationSpeed * DeltaTime;
		SetActorRotation(NewRotation);

		// Bob up and down
		BobTime += DeltaTime * BobSpeed;
		FVector NewLocation = StartLocation;
		NewLocation.Z += FMath::Sin(BobTime) * BobAmplitude;
		SetActorLocation(NewLocation);

		// Pulse scale effect when highlighted
		if (bIsHighlighted && ItemMesh)
		{
			float PulseValue = (FMath::Sin(BobTime * 4.0f) + 1.0f) * 0.5f; // 0 to 1 pulse
			float Scale = 1.2f + (PulseValue * 0.2f); // Scale between 1.2 and 1.4
			ItemMesh->SetRelativeScale3D(FVector(Scale, Scale, Scale));
		}
	}
}

void APickupItem::OnPickedUp_Implementation(AActor* PickingActor)
{
	// Disable further pickups
	bCanBePickedUp = false;
	
	// Hide the item
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// Destroy after a short delay (allows for any effects/sounds to play)
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
		// Enable custom depth for outline effect (if post-process is set up)
		ItemMesh->SetRenderCustomDepth(bHighlight);
		ItemMesh->SetCustomDepthStencilValue(bHighlight ? 1 : 0);

		// Scale up slightly when highlighted for visual feedback
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
