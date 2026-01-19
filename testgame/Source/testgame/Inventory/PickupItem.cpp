// Copyright Epic Games, Inc. All Rights Reserved.

#include "PickupItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

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
	HighlightColor = FLinearColor(0.5f, 1.0f, 0.3f, 1.0f); // Green glow
}

void APickupItem::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();

	// Store original materials and create dynamic instances for highlighting
	if (ItemMesh)
	{
		int32 NumMaterials = ItemMesh->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials; ++i)
		{
			UMaterialInterface* OrigMat = ItemMesh->GetMaterial(i);
			OriginalMaterials.Add(OrigMat);
			
			if (OrigMat)
			{
				UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(OrigMat, this);
				DynamicMaterials.Add(DynMat);
				ItemMesh->SetMaterial(i, DynMat);
			}
			else
			{
				DynamicMaterials.Add(nullptr);
			}
		}
	}
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

		// Pulse highlight effect when highlighted
		if (bIsHighlighted)
		{
			float PulseValue = (FMath::Sin(BobTime * 3.0f) + 1.0f) * 0.5f; // 0 to 1 pulse
			for (UMaterialInstanceDynamic* DynMat : DynamicMaterials)
			{
				if (DynMat)
				{
					DynMat->SetVectorParameterValue(TEXT("EmissiveColor"), HighlightColor * (1.0f + PulseValue * 2.0f));
				}
			}
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

		// Also set emissive color for a glow effect
		for (UMaterialInstanceDynamic* DynMat : DynamicMaterials)
		{
			if (DynMat)
			{
				if (bHighlight)
				{
					DynMat->SetVectorParameterValue(TEXT("EmissiveColor"), HighlightColor);
				}
				else
				{
					DynMat->SetVectorParameterValue(TEXT("EmissiveColor"), FLinearColor::Black);
				}
			}
		}
	}
}
