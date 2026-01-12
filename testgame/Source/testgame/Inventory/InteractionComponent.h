// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class APickupItem;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLookAtItem, APickupItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopLookingAtItem);

/**
 * Component that handles player interaction with pickupable items
 * Performs line traces from the camera to detect items player is looking at
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TESTGAME_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Try to interact with the item we're currently looking at */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool TryInteract();

	/** Get the item we're currently looking at (if any) */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	APickupItem* GetCurrentLookAtItem() const { return CurrentLookAtItem; }

	/** Set the inventory component to add items to */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetInventoryComponent(UInventoryComponent* InInventory) { InventoryComponent = InInventory; }

public:
	/** Called when we start looking at an item */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnLookAtItem OnLookAtItem;

	/** Called when we stop looking at an item */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnStopLookingAtItem OnStopLookingAtItem;

protected:
	/** How far the player can interact with items */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDistance;

	/** Trace channel for interaction line traces */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	/** Reference to the inventory component */
	UPROPERTY()
	UInventoryComponent* InventoryComponent;

private:
	/** Perform line trace to find items */
	void PerformInteractionTrace();

	/** The item we're currently looking at */
	UPROPERTY()
	APickupItem* CurrentLookAtItem;

	/** Previous item we were looking at (for change detection) */
	UPROPERTY()
	APickupItem* PreviousLookAtItem;
};
