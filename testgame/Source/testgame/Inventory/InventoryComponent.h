// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickupItem.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, const TArray<FItemData>&, Items);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, const FItemData&, Item, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemoved, const FItemData&, Item, int32, SlotIndex);

/**
 * Component that manages an inventory of items
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TESTGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	/** Add an item to the inventory. Returns true if successful */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(const FItemData& Item);

	/** Remove an item from a specific slot. Returns true if successful */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(int32 SlotIndex);

	/** Remove a specific quantity of an item by ID. Returns actual amount removed */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 RemoveItemByID(FName ItemID, int32 Quantity = 1);

	/** Get item at a specific slot */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FItemData GetItemAtSlot(int32 SlotIndex) const;

	/** Get all items in the inventory */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FItemData> GetAllItems() const { return Items; }

	/** Check if inventory has space for an item */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasSpaceForItem(const FItemData& Item) const;

	/** Get the total count of a specific item type */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemCount(FName ItemID) const;

	/** Check if inventory contains at least a certain quantity of an item */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(FName ItemID, int32 Quantity = 1) const;

	/** Get current number of items (occupied slots) */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetCurrentItemCount() const;

	/** Get maximum inventory capacity */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetMaxCapacity() const { return MaxCapacity; }

	/** Use an item from a specific slot. Returns true if item was used */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItem(int32 SlotIndex);

public:
	/** Called when the inventory is updated */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	/** Called when an item is added */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemAdded OnItemAdded;

	/** Called when an item is removed */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemRemoved OnItemRemoved;

protected:
	/** Maximum number of item slots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxCapacity;

	/** The items stored in this inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemData> Items;

private:
	/** Find the first slot that can accept the given item (for stacking or empty) */
	int32 FindSlotForItem(const FItemData& Item) const;

	/** Find the first empty slot, returns -1 if none available */
	int32 FindEmptySlot() const;

	/** Broadcast inventory update */
	void BroadcastUpdate();
};
