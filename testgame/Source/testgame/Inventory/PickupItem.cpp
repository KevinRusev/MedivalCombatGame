// Copyright Epic Games, Inc. All Rights Reserved.

#include "PickupItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

APickupItem::APickupItem()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create collision sphere
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(50.f);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = CollisionSphere;

	// Create item mesh
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Default values
	bCanBePickedUp = true;
	RotationSpeed = 90.f;
	BobAmplitude = 10.f;
	BobSpeed = 2.f;
	BobTime = 0.f;
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
	if (ItemMesh)
	{
		ItemMesh->SetRenderCustomDepth(bHighlight);
	}
}
