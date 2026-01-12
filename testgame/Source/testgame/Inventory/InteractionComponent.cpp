// Copyright Epic Games, Inc. All Rights Reserved.

#include "InteractionComponent.h"
#include "PickupItem.h"
#include "InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "testgame.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	InteractionDistance = 300.f;
	TraceChannel = ECC_Visibility;
	CurrentLookAtItem = nullptr;
	PreviousLookAtItem = nullptr;
	InventoryComponent = nullptr;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PerformInteractionTrace();

	// Check for look-at changes
	if (CurrentLookAtItem != PreviousLookAtItem)
	{
		// Unhighlight previous item
		if (PreviousLookAtItem)
		{
			PreviousLookAtItem->SetHighlighted(false);
			OnStopLookingAtItem.Broadcast();
		}

		// Highlight new item
		if (CurrentLookAtItem)
		{
			CurrentLookAtItem->SetHighlighted(true);
			OnLookAtItem.Broadcast(CurrentLookAtItem);
		}

		PreviousLookAtItem = CurrentLookAtItem;
	}
}

bool UInteractionComponent::TryInteract()
{
	if (!CurrentLookAtItem || !CurrentLookAtItem->CanBePickedUp())
	{
		return false;
	}

	// Check if we have an inventory to add to
	if (!InventoryComponent)
	{
		UE_LOG(Logtestgame, Warning, TEXT("InteractionComponent has no InventoryComponent set"));
		return false;
	}

	// Check if inventory has space
	FItemData ItemData = CurrentLookAtItem->GetItemData();
	if (!InventoryComponent->HasSpaceForItem(ItemData))
	{
		UE_LOG(Logtestgame, Log, TEXT("Inventory is full, cannot pick up item: %s"), *ItemData.ItemID.ToString());
		return false;
	}

	// Add item to inventory
	if (InventoryComponent->AddItem(ItemData))
	{
		// Notify the item it was picked up
		CurrentLookAtItem->OnPickedUp(GetOwner());
		
		// Clear current look at item since it's being destroyed
		CurrentLookAtItem = nullptr;
		PreviousLookAtItem = nullptr;
		
		return true;
	}

	return false;
}

void UInteractionComponent::PerformInteractionTrace()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC)
	{
		return;
	}

	// Get camera location and forward vector
	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * InteractionDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		TraceChannel,
		QueryParams
	);

	if (bHit && HitResult.GetActor())
	{
		APickupItem* HitItem = Cast<APickupItem>(HitResult.GetActor());
		if (HitItem && HitItem->CanBePickedUp())
		{
			CurrentLookAtItem = HitItem;
			return;
		}
	}

	CurrentLookAtItem = nullptr;
}
